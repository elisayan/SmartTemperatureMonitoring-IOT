package control.unit.backed;

public interface CommChannel {

    void sendMsg(String msg);

    String receiveMsg();

    boolean isMsgAvailable();
}
