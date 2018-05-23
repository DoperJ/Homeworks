import java.io.*;

public class ReadFile {
    public static String txt2String(File file) {
        StringBuilder result = new StringBuilder();
        try {
            BufferedReader br = new BufferedReader(new FileReader(file));
            String s = null;
            while ((s = br.readLine()) != null) {
                result.append(System.lineSeparator()+s);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result.toString();
    }

    public static void main(String args[]) {
        String name = "江泽群";
        String pathname = "/Users/zeroquest/Desktop/prog/分布式计算/rmi_hello/src/" + name + ".txt";
        File file = new File(pathname);
        System.out.println(txt2String(file));
    }
}