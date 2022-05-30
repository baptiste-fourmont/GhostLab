import java.net.*;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Scanner;
import java.io.*;

public class Client {

    private static Scanner scanner = new Scanner(System.in);

    private static String username;
    private static int port;
    private static int currentClientState;

    private static boolean isOnGame;
    private static int x;
    private static int y;
    private static int number_lines;
    private static int number_columns;
    private static String ip_mutliString;
    private static String ip_to_connect;
    private static int port_multi;
    private static int number_party;
    private static int number_ghost;

    // EN ATTENDANT
    private static String randomUsername() {
        String username = "";
        for (int i = 0; i < 8; i++) {
            username += (char) ((int) (Math.random() * 26) + 97);
        }
        return username;
    }

    // Creer un pseudo aleatoire 'joueurXX' (XX est un nombre), return null si tous
    // les pseudos sont deja pris
    public String generateRandomUsername() {
        String un;
        for (int i = 0; i < 100; i++) { // de 0 a 99
            boolean usernameExists = false;
            un = "joueur";
            if (i < 10) { // creation du pseudo
                un += "0" + i;
            } else {
                un += i;
            } /*
               * for (int j = 0; j < playerList.size(); j++) { // on parcourt la liste des
               * joueurs
               * if (un.equals(playerList.get(j))) {
               * usernameExists = true;
               * break;
               * }
               * }
               */
            if (!usernameExists) {
                return un;
            }
        }
        return null;
    }

    // Creer un port entre 1024 et 9999
    public static int generateRandomPort() {
        int port = (int) (Math.random() * (9999 - 1024) + 1024);
        return port;
    }

    // Demande au joueur de compléter son action
    public static String scanUserAction(int num) {
        String usernameString = "";

        switch (num) {
            case 0: // Nouveau pseudo
                System.out.println("Veuillez choisir un pseudo de 8 charactères.");
                System.out.print("Votre nouveau pseudo : ");
                break;
            case 2: // Se connecter
            case 5: // Recevoir la taille de la grille
            case 6: // Recevoir la liste des joueurs
                System.out.println("Veuillez saisir le numéro de la partie.");
                System.out.print("Partie n° : ");
                break;
            case 8: // Haut
            case 9: // Bas
            case 10: // Gauche
            case 11: // Droite
                System.out.println("De combien de cases souhaitez-vous vous déplacer ?");
                break;
            case 14: // Pseudo du joueur recevant le message
                System.out.println("A qui souhaiter vous envoyer un message ?");
                System.out.print("Pseudo : ");
                usernameString = scanner.nextLine() + " "; // Espace pour l'affichage (ATTENTION)
                System.out.println();
            case 15: // Message à envoyer (Un ou tous les joueurs)
                System.out.println("Veuillez saisir le message que vous souhaitez envoyer.");
                System.out.print("Message : ");
                break;
            default:
                return "";
        }

        String res = scanner.nextLine();
        String userAction = usernameString + res;
        System.out.println();

        return userAction;
    }

    /*
     * 0 = non connecté
     * 1 = connecté et en attente du lancement
     * 2 = connecté et partie commencée
     */
    public static int askUserAction(boolean showDetails) {
        ArrayList<Actions> actions = Actions.addActionToList(currentClientState);
        System.out.println("[GhostLab] > Veuillez choisir le numéro de l'action que vous souhaitez effectuer :");
        int actSize = actions.size();
        System.out.println("==============".repeat(actSize));
        Actions.printActionList(actions, showDetails);

        int num;
        try {
            if (!showDetails) {
                System.out.println("'option' ou 'o' pour voir plus de détails");
            }
            System.out.println("==============".repeat(actSize));
            System.out.print("Votre choix : ");
            String numStr = scanner.nextLine();
            System.out.println();
            if (numStr.toLowerCase().equals("option") || numStr.toLowerCase().equals("o")) {
                return askUserAction(true);
            }
            num = Integer.parseInt(numStr);
        } catch (Exception e) {
            System.out.println("[GhostLab] > Votre choix n'est pas valide.");
            
            return -1; // si c'est pas un chiffre
        }

        if (num < actSize && num >= 0) {
            return actions.get(num).ordinal(); // retourne la 'véritable' valeur de l'action
        }
        System.out.println("[GhostLab] > Votre choix n'est pas valide.");
        return -1; // s'il y a une erreur
    }

    public static String readData(int n, BufferedReader bReader) {
        char[] opcode = new char[n];
        try {
            bReader.read(opcode, 0, n);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new String(opcode);
    }

    public static int guessLength(String opcode) {
        int length = 0;
        switch (opcode) {
            case "GLIS!":
            case "UNROK":
            case "REGOK":
            case "GAMES":
                length = 5;
                break;
            case "LIST!":
            case "OGAME":
                length = 12 - 5;
                break;
            case "SIZE!":
                length = 16 - 5;
                break;
            case "MALL!":
            case "GOBYE":
            case "NSEND":
            case "SEND!":
            case "START":
            case "REGNO":
            case "DUNNO":
                length = 8 - 5;
                break;
            case "PLAYR":
                length = 17 - 5;
                break;
            case "WELCO":
                length = 39 - 5;
                break;
            case "MOVE!":
                length = 16 - 5;
                break;
            case "MOVEF":
                length = 21 - 5;
                break;
            case "GPLYR":
                length = 30 - 5;
                break;
            case "POSIT":
                length = 25 - 5;
                break;
            case "LEAD!":
                length = 22 - 5;
                break;
            default:
                length = 5;
                break;
        }
        return length;
    }

    public static ReceiveCommand receiveCommand(BufferedReader br) {
        String opcString = "";
        String data = "";
        // on lit l'opcode
        opcString = readData(5, br);
        data = opcString;
        data += readData(guessLength(data), br);
        byte[] array = data.getBytes();
        return new ReceiveCommand(opcString, array);
    }

    public static void main(String[] args) throws IllegalArgumentException {
        if (args.length != 3) {
            throw new IllegalArgumentException("java Client [IP] [PORT] [(Verbose) false/true]");
        }
        ip_to_connect = args[0];

        username = randomUsername(); // A changer par generateRandomUsername() quand on pourra verif les pseudo
        port = generateRandomPort();
        currentClientState = 0;
        System.out.println("[GhostLab] > Bienvenue sur GhostLab " + username +" !\n");
        if(args[2].equals("true")){
            System.out.println("Vous avez activé le mode DEBUG");
        }

        try {
            Socket socket = new Socket(args[0], Integer.parseInt(args[1]));
            BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            OutputStream ow = socket.getOutputStream();
            AttenteMultiCast attenteMultiCast = null;
            AttenteUDP au1 = null;
            //boolean clientSentCommand = true;

            while (true) {

                //if (clientSentCommand) {
                    ReceiveCommand cmd = receiveCommand(br);
                    ReceiveCommand.parseCommand(cmd);
                    switch (cmd.getOpcode()) {
                        case "GAMES":
                            byte n = cmd.getBytes()[6];
                            if (n > 0) {
                                for (byte i = 0; i < cmd.getBytes()[6]; i++) {
                                    ReceiveCommand commande2 = receiveCommand(br);
                                    ReceiveCommand.parseCommand(commande2);
                                }
                            }
                            break;
                        case "GLIS!":
                            byte n2 = cmd.getBytes()[6];
                            System.out.println("Number of players: " + n2);
                            if (n2 > 0) {
                                for (byte i = 0; i < n2; i++) {
                                    ReceiveCommand commande2 = receiveCommand(br);
                                    ReceiveCommand.parseCommand(commande2);
                                }
                            }
                            break;
                        case "LIST!":
                            for (byte i = 0; i < cmd.getBytes()[8]; i++) {
                                ReceiveCommand commande2 = receiveCommand(br);
                                ReceiveCommand.parseCommand(commande2);
                            }
                            break;
                        case "WELCO":
                            ReceiveCommand commande2 = receiveCommand(br);
                            ReceiveCommand.parseCommand(commande2);
                            attenteMultiCast = new AttenteMultiCast(port_multi, ip_mutliString);
                            au1 = new AttenteUDP(ip_to_connect, port);
                            break;
                        case "GOBYE":
                            if (au1 != null) {
                                au1.cancel();
                            }
                            if (attenteMultiCast != null) {
                                attenteMultiCast.cancel();
                            }
                            br.close();
                            ow.close();
                            socket.close();
                            System.exit(0);
                            break;
                        default:
                            break;
                    }
                //}

                // Envoie des commandes de la part du client
                System.out.println();
                //clientSentCommand = false;
                int action = -1;
                while (action == -1) {
                    action = askUserAction(false);
                }
                String choice = scanUserAction(action);
                Command command = new Command(action, choice, username, port);
                if (command.getCommand() != null) {
                    String s = new String(command.getCommand(), StandardCharsets.UTF_8);
                    if(args[2].equals("true")){
                        System.out.println("LOG | Commande : '" + s + "'");
                    }
                    ow.write(command.getCommand());
                    //clientSentCommand = true;
                }

            }
        } catch (Exception e) {
            /*
             * System.out.println(e);
             * e.printStackTrace();
             */
            System.out.println("\nIl y a eu une erreur entre le client et le serveur.");
            System.out.println(
                    "Veuillez vérifier que le serveur est bien lancé et que vous avez bien entré les bonnes informations.");
        }
    }

    public static void setOnGame(boolean onGame) {
        isOnGame = onGame;
    }

    public static void setGhost(int ghost) {
        number_ghost = ghost;
    }

    public static void setLine(int line) {
        number_lines = line;
    }

    public static void setColumns(int columns) {
        number_columns = columns;
    }

    public static void setX(int x2) {
        x = x2;
    }

    public static void setY(int y2) {
        y = y2;
    }

    public static void setIP(String mulString) {
        ip_mutliString = mulString;
    }

    public static void setParty(int party) {
        number_party = party;
    }

    public static void setPort(int port2) {
        port_multi = port2;
    }

    public static String getIP() {
        return ip_mutliString;
    }

    public static int getGhost() {
        return number_ghost;
    }

    public static boolean getIsOnGame() {
        return isOnGame;
    }

    public static int getPortMulti() {
        return port_multi;
    }

    public static int getX() {
        return x;
    }

    public static int getY() {
        return y;
    }

    public static int getParty() {
        return number_party;
    }

    public static int getLines() {
        return number_lines;
    }

    public static int getColumns() {
        return number_columns;
    }

    public static void setCurrentClientState(int currentClientState) {
        Client.currentClientState = currentClientState;
    }

    public static int getCurrentClientState() {
        return currentClientState;
    }

}