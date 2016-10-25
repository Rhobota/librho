import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;

public class UdpBroadcaster {

private static final int PORT = 12347;
private static final String MCAST_ADDR = "FF7E:230::1234";

private static InetAddress GROUP;

public static void main(String[] args) {
    try {
        GROUP = InetAddress.getByName(MCAST_ADDR);
        DatagramSocket serverSocket = null;
        try {
            serverSocket = new DatagramSocket();
            try {
                while (true) {
                    byte[] sendData = new byte[5];
                    sendData[0] = 'H';
                    sendData[1] = 'e';
                    sendData[2] = 'l';
                    sendData[3] = 'l';
                    sendData[4] = 'o';
                    DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, GROUP, PORT);
                    serverSocket.send(sendPacket);
                    Thread.sleep(1000);
                }
            } catch (Exception e) {
                System.out.println(e);
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    } catch (Exception e) {
        System.out.println("Usage : [group-ip] [port]");
    }
}

}
