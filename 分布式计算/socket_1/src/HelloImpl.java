import java.rmi.*;
import java.rmi.server.*;

public class HelloImpl extends UnicastRemoteObject implements HelloInterface{
    public HelloImpl() throws RemoteException{
        super();
    }
    public String sayHello (String name) throws RemoteException {
        return "Welcome to RMI !" + name;
    }
}
