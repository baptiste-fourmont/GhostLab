import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class Command {
    private String username, choice;
    private int port, action;
    private byte[] command;

    public Command(int a, String c, String u, int p) {
        this.action = a;
        this.choice = c;
        this.username = u;
        this.port = p;
        if (validCommand()) {
            System.out.println();
            command = transfromIntoCommand();
        }
    }

    // Vérifier si le string est alpha-numérique
    public static boolean isAlphaNumeric(String s) {
        return s.matches("[a-zA-Z0-9]+");
    }

    // Vérifier les choix de l'utilisateur et la commande
    public boolean validCommand() {
        switch (action) {
            case 0: // Nouveau pseudo
                if (choice.length() == 8 && isAlphaNumeric(choice)) {
                    return true;
                }
                break;
            case 2: // regis
            case 5: // size?
            case 6: // list?
                try {
                    int n = Integer.parseInt(choice);
                    if (n >= 0 && n < 256) {
                        return true;
                    }
                } catch (NumberFormatException e) {
                    return false;
                }
                break;
            case 8: // Haut
            case 9: // Bas
            case 10: // Gauche
            case 11: // Droite
                try {
                    int n = Integer.parseInt(choice);
                    if (n > 0 && n < 1000) {
                        return true;
                    }
                } catch (NumberFormatException e) {
                    return false;
                }
                break;
            case 14: // Un joueur
                try {
                    String strUser = choice.substring(0, 8);
                    if (isAlphaNumeric(strUser) && choice.length() <= 209 && !choice.contains("+++")
                            && !choice.contains("***")) { // 8 (pseudo) + 1 (espace) + 200 (message)
                        return true;
                    }
                } catch (Exception e) {
                    return false;
                }
                break;
            case 15: // Tous les joueurs
                if (choice.length() <= 200 && !choice.contains("+++") && !choice.contains("***")) {
                    return true;
                }
                break;
            default:
                if (choice.equals("")) {
                    return true;
                }
                break;
        }
        return false;
    }

    // Transformer les choix de l'utilisateur en une commande
    public byte[] transfromIntoCommand() {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        Integer nb;
        byte b;
        String comm = "";
        switch (action) {
            case 0: // Nouveau pseudo
                break;
            case 1: // NEWPL
                comm = "NEWPL " + username + " " + port;
                break;
            case 2: // REGIS
            case 5: // SIZE?
            case 6: // LIST?
                try {
                    comm = Actions.values()[action].getOpcode() + " ";
                    if(action == 2){
                        comm +=  username + " " + port + " ";
                    }
                    baos.write(comm.getBytes());
                    nb = Integer.parseInt(choice);
                    b = (byte) nb.intValue();
                    baos.write(b);
                    baos.write("***".getBytes());
                    return baos.toByteArray();
                } catch (IOException e) {
                    System.out.println("Impossible de créer la commande.");
                    return null;
                }
            case 3: // START
            case 4: // UNREG
            case 7: // GAME?
            case 12: // IQUIT
            case 13: // GLIS?
            case 16:
                comm = Actions.values()[action].getOpcode();
                break;
            case 14: // SEND?
            case 15: // MALL?
                comm = Actions.values()[action].getOpcode() + " " + choice;
                break;
            case 8: // UPMOV
            case 9: // DOMOV
            case 10: // LEMOV
            case 11: // RIMOV
                comm = Actions.values()[action].getOpcode() + " ";
                if(Integer.parseInt(choice) <= 9){
                    comm += "00"+choice;
                }else if(Integer.parseInt(choice) <= 99){
                    comm += "0"+choice;
                }else{
                    comm += choice;
                }
                break;
            default:
                return null;
        }

        comm = comm.concat("***"); // Ajouter 3 étoiles pour séparer les commandes
        if (comm.equals("***")) {
            return null;
        }

        return comm.getBytes();
    }

    public byte[] getCommand() {
        return command;
    }

}