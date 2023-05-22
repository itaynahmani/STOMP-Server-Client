package bgu.spl.net.impl.stomp;

import java.util.LinkedList;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol {

    protected ConnectionsImpl<String> serverConnections;
    protected int myConnectionID;
    protected boolean shouldTerminate;
    protected String myUser;
    protected int msgId;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        myConnectionID = connectionId;
        serverConnections = (ConnectionsImpl) connections;
        shouldTerminate = false;
        myUser = null;
        msgId = 1;

    }

    @Override
    public void process(String message) {
        String[] parts = message.split("\n");
        String receipt = returnHeader("receipt", parts);
        String command = parts[0];

        if (command.equals("CONNECT")) {
            connect(receipt, parts);
        } 
        else if (command.equals("SEND")) {
            send(receipt, parts);
        } 
        else if (command.equals("SUBSCRIBE")) {
            subscribe(receipt, parts);
        } 
        else if (command.equals("UNSUBSCRIBE")) {
            unsubscribe(receipt, parts);
        } 
        else if (myUser != null && command.equals("DISCONNECT")) {
            disconnect(receipt);
        }
        else{
            sendErrorFrame("the command is illegal ", receipt, myConnectionID); 
        }


    }

    // ----------------------- Command Func ------------------


    public void connect(String receipt, String[] parts){

        String version = returnHeader("accept-version", parts);
        String username = returnHeader("login", parts);
        String password = returnHeader("passcode", parts);
        if (version.equals("") | username.equals("") | password.equals("")) {
            sendErrorFrame("message: malformed frame received", receipt, myConnectionID);
        } else {
            if (serverConnections.UsersToPasswords.get(username) == null) {
                // add this new username and password
                myUser = username;
                serverConnections.UsersToPasswords.put(username, password);
                serverConnections.UsersToClients.put(username, myConnectionID);
                serverConnections.ClientsToUsers.put(myConnectionID, username);
                String StompFrame = ConnectedFrame(version);
                serverConnections.ConnectionHandlers.get(myConnectionID).send(StompFrame);

            } else if (serverConnections.UsersToClients.get(username) != null) {
                // send ERROR: user taken
                sendErrorFrame("message: User already logged in", receipt, myConnectionID);

            } else if (!serverConnections.UsersToPasswords.get(username).equals(password)) {
                // send ERROR: wrong password
                sendErrorFrame("message: Wrong Password", receipt, myConnectionID);

            } else {
                // connect the client and the username.
                myUser = username;
                serverConnections.UsersToClients.put(username, myConnectionID);
                serverConnections.ClientsToUsers.put(myConnectionID, username);
                String StompFrame = ConnectedFrame(version);
                serverConnections.ConnectionHandlers.get(myConnectionID).send(StompFrame);
            }
        }
    }


    public void send(String receipt, String[] parts){
        String topic = returnHeader("destination", parts);
        if (topic.equals("")) {
            sendErrorFrame("malformed frame received", receipt, myConnectionID);
        } else {
            String frameBody = getBody(parts);
            System.out.println("the frame body:" + frameBody);

            LinkedList<String> users = serverConnections.TopicsToUsers.get(topic);
            System.out.println(users);
            if (users != null && users.contains(myUser)) {
                for (String u : users) {
                    String SubId = serverConnections.PairToSubId.get(u).get(topic);
                    int clientID = serverConnections.UsersToClients.get(u);
                    System.out.println("going to sendMessage");
                    sendMessage(topic, frameBody, SubId, clientID);
                }
            } else {
                System.out.println("else");
                sendErrorFrame("message: you are need to subscribe first.", receipt, myConnectionID);
            }
        }
    }


    public void subscribe(String receipt, String[] parts){

        String topic = returnHeader("destination", parts);
        String subscriptionID = returnHeader("id", parts);
        if (topic.equals("") | subscriptionID.equals("")) {
            sendErrorFrame("malformed frame received", receipt, myConnectionID);
        } else {            
            LinkedList<String> usersList = serverConnections.TopicsToUsers.get(topic);
            if (usersList == null) {
                LinkedList<String> L = new LinkedList<>();
                L.add(myUser);
                serverConnections.TopicsToUsers.put(topic, L);
                serverConnections.addSubscribe(myUser, topic, subscriptionID);

            } else {
                usersList.add(myUser);
                serverConnections.TopicsToUsers.put(topic, usersList);
                serverConnections.addSubscribe(myUser, topic, subscriptionID);
                }
            LinkedList<String> subs = serverConnections.UsersToSubId.get(myUser);
            if (subs == null) {
                subs = new LinkedList<>();
            }
            subs.add(subscriptionID);
            serverConnections.UsersToSubId.put(myUser, subs);
            String msg = receipt(receipt, 0);
            System.out.println(topic);
            System.out.println(serverConnections.TopicsToUsers.get(topic));
            serverConnections.send(myConnectionID, msg);
            }

    }


    public void unsubscribe(String receipt, String[] parts){

        String subscriptionID = returnHeader("id", parts);
        if (subscriptionID.equals("")) {
            sendErrorFrame("malformed frame received", receipt, myConnectionID);
        } else {
            String topic = serverConnections.unsubscribe(myUser, subscriptionID);
            if (topic == null) {
                // ERROR: cant unsubscribe if you are not subscribed
                sendErrorFrame("message: Illegal command. Subscription Not Found.", receipt, myConnectionID);
            } else {
                LinkedList<String> L = serverConnections.TopicsToUsers.get(topic);
                L.remove(myUser);
                serverConnections.TopicsToUsers.put(topic, L);

                LinkedList<String> L2 = serverConnections.UsersToSubId.get(myUser);
                L2.remove(subscriptionID);
                serverConnections.UsersToSubId.put(myUser, L2);

                String msg = receipt(receipt, 0);
                serverConnections.send(myConnectionID, msg);
            }
        }
    }

    public void disconnect(String receipt){

        System.out.println("enter disconnect");
        removeBeforeDisconnect();
        String msg = receipt(receipt, 1);
        System.out.println("before send msg of disconnect");
        serverConnections.ConnectionHandlers.get(myConnectionID).send(msg);

    }


    // ----------------------- Command Func ------------------



    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    // sends receipt frame
    private String receipt(String receipt, int type) {
        String frameReceipt;
        if (type == 0)
            frameReceipt = "RECEIPT" + "\n" + "receipt-id:" + receipt + "\n";
        else
            frameReceipt = "RECEIPT" + "\n" + "receipt-id:" + receipt + "\n";

        return frameReceipt;
    }

    private void removeBeforeDisconnect(){
        serverConnections.ClientsToUsers.remove(myConnectionID);
        serverConnections.UsersToClients.remove(myUser);
        LinkedList<String> L = serverConnections.UsersToSubId.remove(myUser);
        if (L != null) {
            for (String subId : L) {
                String topic = serverConnections.PairToTopic.get(myUser).get(subId);
                LinkedList<String> Users = serverConnections.TopicsToUsers.get(topic);
                Users.remove(myUser);
                serverConnections.TopicsToUsers.put(topic, Users);
            }
            serverConnections.PairToTopic.remove(myUser);
            serverConnections.PairToSubId.remove(myUser);
        }
        myUser = null;
    }

    private void sendErrorFrame(String err, String rec, int clientID) {
        removeBeforeDisconnect();
        String frameError = "ERROR\n";
        if (rec.length() > 0)
            frameError += "receipt-id:" + rec + "\n";
        frameError += err + "\n";
        serverConnections.ConnectionHandlers.get(clientID).send(frameError);
        shouldTerminate = true;
    }

    private String ConnectedFrame(String version) {
        String connectedFrame = "CONNECTED" + "\n" + "version:" + version + "\n" + "\n";
        return connectedFrame;
    }

    private String sendMessage(String destination, String frameBody, String subId, int client) {

        String message = "MESSAGE" + "\n";
        message = message + "subscription:" + subId + "\n";
        message = message + "message-id:" + msgId + "\n";
        message = message + "destination:" + destination + "\n";
        message = message + "\n";
        message = message + frameBody + "\n";

        msgId++;
        System.out.println("going to serverConnections.send");
        serverConnections.send(client, message);

        return message;

    }

    private String returnHeader(String header, String[] lines) {
        for (String line : lines) {
            String[] s = line.split(":");
            if (s[0].equals(header))
                return s[1];
        }
        return "";
    }

    private String getBody(String[] parts) {
        int index = 0;
        String body = "";
        for (int i = 0; i < parts.length; i++) {
            String[] s = parts[i].split(":");
            if (s[0].equals("user")) {
                index = i;
                break;
            }
        }
        for (int i = index; i < parts.length; i++) {
            body = body + parts[i] + '\n';
        }
        return body;

    }

}