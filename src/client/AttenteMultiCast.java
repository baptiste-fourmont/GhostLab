import java.net.MulticastSocket;
import java.net.InetAddress;
import java.net.DatagramPacket;

public class AttenteMultiCast implements Runnable {
    private int port;
    private String adresse;
    Thread t;

    protected byte[] buffer = new byte[1024];

    public AttenteMultiCast(int port, String adresse) {
        this.port = port;
        this.adresse = adresse;
        t = new Thread(this);
        t.start();
    }

    public void cancel() {
       t.interrupt();
    }

    @Override
    public void run() {
        try (MulticastSocket socket = new MulticastSocket(port)){
            InetAddress group = InetAddress.getByName(adresse);
            socket.joinGroup(group);
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
            while (!Thread.interrupted()) {
                socket.receive(packet);
                String message = new String(packet.getData(), 0, packet.getLength());
                message = message.substring(0, message.length() - 3);
                String[] array = message.split(" ");
                if (message.substring(0, 6).equals("SCORE ")) {
                    System.out.println("Message multicast - " + array[0] + " de " + array[1] + " avec " + Integer.parseInt(array[2]) + " points, il est en POS(" + Integer.parseInt(array[3]) + "," + Integer.parseInt(array[4]) + ")");
                } else if (message.substring(0, 6).equals("GHOST ")) {
                    System.out.println("[GhostLab] > Un fantome a bougé en position (" + Integer.parseInt(array[1])
                    + ", " + Integer.parseInt(array[2]) + ")");
                }else if(message.substring(0, 6).equals("MESSA ")){
                    System.out.println("["+ array[1] + "] à tout le monde > " + message.substring(14, message.length()));
                }else if(message.substring(0, 6).equals("ENDGA ")){
                    System.out.println("[GhostLab] > La partie est terminée");
                    System.out.println("[GhostLab] > Le Vainqueur est: " + array[1] + " avec "
                    + Integer.parseInt(array[2]) + " points");
                } else {
                    System.out.println("[GhostLab] > Message multicast recu non conforme : " + message);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
