import java.rmi.*;
import java.rmi.server.*;
import java.util.*;

public class HelloImpl extends UnicastRemoteObject implements HelloInterface{
    public HelloImpl() throws RemoteException {
        super();
    }
    public String sayHello(String name) throws RemoteException {
        new myThread().run();
        return "Welcome to RMI, " + name;
    }
}


class myThread extends Thread {
    public myThread() {
        super();
    }
    public void run() {
        try {
            sleep(10000);
        }
        catch(InterruptedException ie) {
            ie.printStackTrace();
        }
    }
}
