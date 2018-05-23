import java.net.InetAddress;
import java.net.DatagramSocket;
import java.net.DatagramPacket;
import java.io.*;


public class Example1Sender {
    public static void main(String args[]) {
        if (args.length != 3)
            System.out.print("This program requires three command line arguments!");
        else {
            try {
                InetAddress recieverHost = InetAddress.getByName(args[0]);
                int recieverPort = Integer.parseInt(args[1]);
                String message = args[2];
                //instantiate a datagram socket for sending the data
                DatagramSocket mySocket = new DatagramSocket();
                byte[] buffer = message.getBytes();
                //A complete packet to be sent requires arguments including a byte array, length of it,
                // address of the reciever host, the port of it.
                DatagramPacket datagram = new DatagramPacket(buffer, buffer.length, recieverHost, recieverPort);
                //mySocket.setSoTimeout(3000);
                mySocket.send(datagram);
                mySocket.close();
            }
            catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }
}