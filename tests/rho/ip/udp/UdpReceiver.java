import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;

public class UdpReceiver {

private static final int PORT = 12347;
private static final String MCAST_ADDR = "FF7E:230::1234";

private static InetAddress GROUP;

public static void main(String[] args) {
    try {
        GROUP = InetAddress.getByName(MCAST_ADDR);
        MulticastSocket multicastSocket = null;
        try {
            multicastSocket = new MulticastSocket(PORT);
            multicastSocket.joinGroup(GROUP);
            while (true) {
                try {
                    byte[] receiveData = new byte[256];
                    DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
                    multicastSocket.receive(receivePacket);
                    System.out.println("Client received from : " + receivePacket.getAddress() + ", " + new String(receivePacket.getData(), "UTF-8"));
                } catch (Exception e) {
                    System.out.println(e);
                }
            }
        } catch (Exception e) {
            System.out.println(e);
        } finally {
            multicastSocket.close();
        }
    } catch (Exception e) {
        System.out.println("Usage : [group-ip] [port]");
    }
}

}
