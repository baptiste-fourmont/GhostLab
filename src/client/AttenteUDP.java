import java.net.InetAddress;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
public class AttenteUDP implements Runnable{
    private int port;
    private String ip;
    Thread t;

    protected byte[] buffer = new byte[1024];

    public AttenteUDP(String ip, int port){
        this.ip = ip;
        this.port = port;
        t = new Thread(this);
        t.start();
    }

    public void cancel() {
        t.interrupt();
    }

    @Override
    public void run(){
        try(DatagramSocket socket = new DatagramSocket(port, InetAddress.getByName(ip))){
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
            while(!Thread.interrupted()){
                socket.receive(packet);
                String message = new String(packet.getData(), 0, packet.getLength());
                message = message.substring(0, message.length() - 3);
                if(message.substring(0, 6).equals("MESSP ")){
                    //System.out.println("Message UDP de " + message.substring(6, 14) + " : "
                            //+ message.substring(14, message.length() - 3));
                            String[] array = message.split(" ");
                            System.out.println("[" + array[1] + "] à vous > ");
                            for(int i = 2; i < array.length; i++){
                                System.out.print(array[i] + " ");
                            }
                            System.out.println();
                }else{
                    System.out.println("[GhostLab] > Message privé recu non conforme : " + message);
                }
            }
        }catch(Exception e){
            e.printStackTrace();
        }
    }
}
