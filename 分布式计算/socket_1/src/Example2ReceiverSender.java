import java.net.*;

public class Example2ReceiverSender {
    public static void main(String args[]) {
        if (args.length != 4) {
            System.out.println("This program requires four command line arguments!");
        }
        else {
            try {
                InetAddress receiverAddress = InetAddress.getByName(args[0]);
                int receiverPort = Integer.parseInt(args[1]);
                int myPort = Integer.parseInt(args[2]);
                String message = args[3];
                MyDatagramSocket mySocket = new MyDatagramSocket(myPort);
                while (true) {
                    System.out.println(mySocket.receiveMessage());
                    mySocket.sendMessage(receiverAddress, receiverPort, message);
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
