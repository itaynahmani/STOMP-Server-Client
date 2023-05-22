package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import java.util.*;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T> {

    public ConcurrentHashMap<Integer, ConnectionHandler<T>> ConnectionHandlers = new ConcurrentHashMap<>();
    protected ConcurrentHashMap<String, Integer> UsersToClients = new ConcurrentHashMap<>();
    protected ConcurrentHashMap<Integer, String> ClientsToUsers = new ConcurrentHashMap<>();
    protected ConcurrentHashMap<String, String> UsersToPasswords = new ConcurrentHashMap<>();
    protected ConcurrentHashMap<String, LinkedList<String>> TopicsToUsers = new ConcurrentHashMap<>();
    protected ConcurrentHashMap<String, LinkedList<String>> UsersToSubId = new ConcurrentHashMap<>();

    protected ConcurrentHashMap<String, ConcurrentHashMap<String, String>> PairToTopic = new ConcurrentHashMap<>();
    protected ConcurrentHashMap<String, ConcurrentHashMap<String, String>> PairToSubId = new ConcurrentHashMap<>();

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> CH = ConnectionHandlers.get(connectionId);
        if (CH != null) {
            CH.send(msg);
            System.out.println("going to send msg from the CH:" + msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, T msg) {
        LinkedList<String> subscribedUsers = TopicsToUsers.get(channel);
        if (subscribedUsers != null) {
            for (String user : subscribedUsers) {
                int clientID = UsersToClients.get(user);
                ConnectionHandler<T> CH = ConnectionHandlers.get(clientID);
                if (CH == null)
                    UsersToClients.remove(user, subscribedUsers);// check if necessary
                else
                    CH.send(msg);
            }
        }

    }

    @Override
    public void disconnect(int connectionId) {
        ConnectionHandlers.remove(connectionId);

    }

    public String getUser(int id) {
        return ClientsToUsers.get(id);
    }

    public ConnectionHandler getHandler(int id) {
        return ConnectionHandlers.get(id);
    }
    public void addSubscribe(String user, String topic, String subID) {
        if(PairToTopic.get(user)==null){
            ConcurrentHashMap subIdToTopic = new ConcurrentHashMap<>();
            subIdToTopic.put(subID, topic);
            PairToTopic.put(user, subIdToTopic);
        }

        else{
            PairToTopic.get(user).put(subID, topic);
        }

        if(PairToSubId.get(user)==null){
            ConcurrentHashMap TopicToSubID = new ConcurrentHashMap<>();
            TopicToSubID.put(topic, subID);
            PairToSubId.put(user, TopicToSubID);
        }
        else{
            PairToSubId.get(user).put(subID, topic);
        }
           
    }
    public String unsubscribe(String user, String subID) {
        String topic;
        if(PairToTopic.get(user)==null){
            topic=null;
        }
        else if(PairToTopic.get(user).get(subID)==null){
            topic=null;
        }
        else{
            topic = PairToTopic.get(user).remove(subID);
            PairToSubId.get(user).remove(topic);
        }
        return topic;
    }



}