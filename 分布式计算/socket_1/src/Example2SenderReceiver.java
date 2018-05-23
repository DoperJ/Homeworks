import java.net.*;

public class Example2SenderReceiver {
    public static void main(String args[]) {
        if (args.length != 4) {
            System.out.println("This programm requires four command line arguments!");
        }
        else {
            try {
                InetAddress receiverHost = InetAddress.getByName(args[0]);
                int receiverPort = Integer.parseInt(args[1]);
                int myPort = Integer.parseInt(args[2]);
                String message = args[3];
                MyDatagramSocket mySocket = new MyDatagramSocket(myPort);
                while (true) {
                    mySocket.sendMessage(receiverHost, receiverPort, message);
                    System.out.println(mySocket.receiveMessage());
                    Thread.sleep(3000);
                }
                //mySocket.close();
            }
            catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }
}
