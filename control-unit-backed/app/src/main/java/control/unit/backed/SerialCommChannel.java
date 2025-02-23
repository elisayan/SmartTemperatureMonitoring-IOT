package control.unit.backed;

import java.util.concurrent.*;
import jssc.*;

public class SerialCommChannel implements CommChannel, SerialPortEventListener {
    private final SerialPort serialPort;
    private final BlockingQueue<String> queue;
    private StringBuffer currentMsg = new StringBuffer();

    public SerialCommChannel(String port, int rate) throws Exception {
        queue = new ArrayBlockingQueue<>(100);
        serialPort = new SerialPort(port);
        serialPort.openPort();
        serialPort.setParams(rate, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
        serialPort.addEventListener(this);
    }

    @Override
    public void sendMsg(String msg) {
        try {
            serialPort.writeBytes((msg + "\n").getBytes());
        } catch (SerialPortException ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public String receiveMsg() throws InterruptedException {
        return queue.take();
    }

    @Override
    public boolean isMsgAvailable() {
        return !queue.isEmpty();
    }

    public void close() {
        try {
            serialPort.closePort();
        } catch (SerialPortException ex) {
            ex.printStackTrace();
        }
    }

    public void serialEvent(SerialPortEvent event) {
        if (event.isRXCHAR()) {
            try {
                String msg = serialPort.readString(event.getEventValue());
                currentMsg.append(msg.replaceAll("\r", ""));

                while (true) {
                    String msg2 = currentMsg.toString();
                    int index = msg2.indexOf("\n");
                    if (index >= 0) {
                        queue.put(msg2.substring(0, index));
                        currentMsg = new StringBuffer(msg2.substring(index + 1));
                    } else {
                        break;
                    }
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }
}
