package control.unit.backed;

import io.vertx.core.Vertx;
import jssc.SerialPort;
import jssc.SerialPortException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.ArrayBlockingQueue;

public class SerialCommChannel implements CommChannel {
    private final SerialPort serialPort;
    private final BlockingQueue<String> queue;
    private final Vertx vertx;

    public SerialCommChannel(Vertx vertx, String port, int rate) throws Exception {
        this.vertx = vertx;
        this.queue = new ArrayBlockingQueue<>(1000);
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
        }, false).onComplete(result -> {
            if (result.failed()) {
                System.err.println("Errore invio messaggio: " + result.cause().getMessage());
            }
        });
    }

    @Override
    public String receiveMsg() throws InterruptedException {
        return queue.poll(1, TimeUnit.SECONDS);
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
                    boolean added = queue.offer(msg.replaceAll("\r", ""));
                    if (!added) {
                        System.err.println("Coda piena: messaggio scartato.");
                    }
                    promise.complete();
                } catch (Exception ex) {
                    promise.fail(ex);
                }
            }).onComplete(result -> {
                if (result.failed()) {
                    System.err.println("Errore lettura messaggio: " + result.cause().getMessage());
                }
            });
        }
    }

    public void close() {
        try {
            serialPort.closePort();
        } catch (SerialPortException ex) {
            ex.printStackTrace();
        }
    }
}