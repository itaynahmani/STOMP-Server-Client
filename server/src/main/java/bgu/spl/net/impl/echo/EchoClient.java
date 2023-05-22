package bgu.spl.net.impl.echo;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class EchoClient {

    public static void main(String[] args) throws IOException {

        String testSUB = "SUBSCRIBE\ndestination:/germany_spain\nid:17\nreceipt:73\n\n\u0000";
        String testCON = "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:meni\npasscode:films\n\n\u0000";
        String testUNS = "UNSUBSCRIBE\nid:17\nreceipt:82\n\n\u0000";
        String testDIS = "DISCONNECT\nreceipt:113\n\n\u0000";
        String testSND = "SEND\nreceipt:100\ndestinaton:/germany_spain\n\nitaythekingggggg\n\n\u0000";

        String testCON2 = "CONNECT\naccept-version:1.2\nreceipt:82\nhost:stomp.cs.bgu.ac.il\nlogin:meni\npasscode:film\n\n\u0000";

        if (args.length == 0) {
            args = new String[] { "localhost", testCON };
        }

        if (args.length < 2) {
            System.out.println("you must supply two arguments: host, message");
            System.exit(1);
        }

        // BufferedReader and BufferedWriter automatically using UTF-8 encoding
        try (Socket sock = new Socket(args[0], 7777);
                BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
                BufferedWriter out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()))) {

            System.out.println("sending  CONNECT message to server");
            out.write(args[1]);
            //out.newLine();
            out.flush();

            
             
            String line = in.readLine();
            System.out.println("message from server: ");
            System.out.println(line);
            line = in.readLine();
            System.out.println(line);
            line = in.readLine();
            System.out.println(line);
           

            System.out.println("sending  DISCON message to server");
            out.write(testDIS);
            out.flush();

            System.out.println("message from server: ");
            System.out.println(line);
            line = in.readLine();
            System.out.println(line);
            line = in.readLine();
            System.out.println(line);

            System.out.println("sending  CONNECT message to server");
            out.write(testCON2);
            out.flush();


            System.out.println("message from server: ");
            line = in.readLine();
            while(line!=null){
                System.out.println(line);
                line = in.readLine();

            }

            // System.out.println("sending  UNSUBSCRIBE message to server");
            // out.write(testUNS);
            // out.flush();

            // System.out.println("message from server: ");
            // line = in.readLine();
            // System.out.println(line);
            // line = in.readLine();
            // System.out.println(line);

            // System.out.println("sendind subscribe:");
            // out.write(testSUB);
            // // System.out.println("writing to args1 \n" + args[1] + "end of");
            // out.newLine();
            // out.flush();

            // System.out.println("message from server: ");
            // while(line!=null){
            //     System.out.println(line);
            //     line = in.readLine();

            // }


            
        }
    }
}

