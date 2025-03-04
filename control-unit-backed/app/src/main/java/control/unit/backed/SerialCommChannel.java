package control.unit.backed;

import io.vertx.core.Vertx;
import jssc.SerialPort;
import jssc.SerialPortException;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.Queue;

public class SerialCommChannel implements CommChannel {
    private final SerialPort serialPort;
    private final Queue<String> queue;
    private final Vertx vertx;
    private StringBuffer currentMsg = new StringBuffer();

    public SerialCommChannel(Vertx vertx, String port, int rate) throws Exception {
        this.vertx = vertx;
        this.queue = new ConcurrentLinkedQueue<>();
        this.serialPort = new SerialPort(port);
        this.serialPort.openPort();
        this.serialPort.setParams(rate, 8, 1, 0);
        this.serialPort.addEventListener((event) -> handleSerialEvent(event));
    }

    @Override
    public void sendMsg(String msg) {
        vertx.executeBlocking(promise -> {
            try {
                serialPort.writeBytes((msg + "\n").getBytes());
                promise.complete();
            } catch (SerialPortException ex) {
                promise.fail(ex);
            }
        }, false, res -> {
            if (res.failed()) {
                System.err.println("Errore invio: " + res.cause().getMessage());
            }
        });
    }

    @Override
    public String receiveMsg() {
        return queue.poll();
    }

    @Override
    public boolean isMsgAvailable() {
        return !queue.isEmpty();
    }

    private void handleSerialEvent(jssc.SerialPortEvent event) {
        if (event.isRXCHAR()) {
            vertx.executeBlocking(promise -> {
                try {
                    String msg = serialPort.readString(event.getEventValue());
                    msg = msg.replaceAll("\r", "");
                    currentMsg.append(msg);
                    processBuffer();
                    promise.complete();
                } catch (Exception ex) {
                    promise.fail(ex);
                }
            }, false, res -> {
                if (res.failed()) {
                    System.err.println("Errore lettura: " + res.cause().getMessage());
                }
            });
        }
    }

    private void processBuffer() {
        String buffer = currentMsg.toString();
        int index;
        while ((index = buffer.indexOf("\n")) >= 0) {
            String message = buffer.substring(0, index);
            queue.offer(message); // Non bloccante
            buffer = buffer.substring(index + 1);
        }
        currentMsg = new StringBuffer(buffer);
    }

    public void close() {
        try {
            serialPort.closePort();
        } catch (SerialPortException ex) {
            ex.printStackTrace();
        }
    }
}