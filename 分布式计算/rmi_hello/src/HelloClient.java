import java.rmi.*;
import java.io.*;
import java.util.*;

public class HelloClient {
    public static void main(String args[]) {
        try {
            int RMIPort;
            String hostName;
            InputStreamReader is = new InputStreamReader(System.in);
            BufferedReader br = new BufferedReader(is);
            System.out.println("Enter the RMIRegistry host name: ");
            hostName = br.readLine();
            System.out.println("Enter the RMIRegistry port number: ");
            String portNum = br.readLine();
            RMIPort = Integer.parseInt(portNum);
            String registryURL = "rmi://" + hostName + ":" + portNum + "/hello";
            HelloInterface h = (HelloInterface) Naming.lookup(registryURL);
            System.out.print("Thread started at: ");
            System.out.println(new Date(System.currentTimeMillis()));
            String message = h.sayHello("DoperJ");
            System.out.println("HelloClient: " + message);
            System.out.print("Thread ended at: ");
            System.out.println(new Date(System.currentTimeMillis()));
        }
        catch(Exception ex) {
            System.out.println("Exception in HelloClient" + ex);
        }

    }
}
