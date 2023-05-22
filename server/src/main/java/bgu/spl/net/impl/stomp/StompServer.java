package bgu.spl.net.impl.stomp;

import java.io.IOException;

import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        //you can use any server...
        if(args[0].equals("tpc")){
        Server.threadPerClient(
                7777, // port
                () -> new StompMessagingProtocolImpl(), // protocol factory
                () -> new StompMessageEncoderDecoder() // message encoder decoder factory
        ).serve();
        }
        
        else if(args[0].equals("reactor")){
            Server.reactor(
            Runtime.getRuntime().availableProcessors(),
            7777, // port
            () -> new StompMessagingProtocolImpl(), // protocol factory
            StompMessageEncoderDecoder::new // message encoder decoder factory
            ).serve();
        }
    }
}
