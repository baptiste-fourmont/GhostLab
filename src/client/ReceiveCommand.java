public class ReceiveCommand {
    private final String opcode;
    private byte[] data;
    public final int dataLength;

    public ReceiveCommand(String opcode, byte[] data) {
        this.opcode = opcode;
        switch (opcode) {
            case "GLIS!":
            case "REGOK":
            case "UNROK":
            case "GAMES":
                this.dataLength = 10;
                break;
            case "LIST!":
            case "OGAME":
                this.dataLength = 12;
                break;
            case "SIZE!":
                this.dataLength = 16;
                break;
            case "MALL!":
            case "GOBYE":
            case "NSEND":
            case "SEND!":
            case "START":
            case "REGNO":
            case "DUNNO":
                this.dataLength = 8;
                break;
            case "WELCO":
                this.dataLength = 39;
                break;
            case "PLAYR":
                this.dataLength = 17;
                break;
            case "MOVE!":
                this.dataLength = 16;
                break;
            case "POSIT":
                this.dataLength = 25;
                break;
            case "MOVEF":
                this.dataLength = 21;
                break;
            case "GPLYR":
                this.dataLength = 29;
                break;
            case "LEAD!":
                this.dataLength = 22;
                break;
            default:
                this.dataLength = 5;
        }
        this.data = data;
    }

    public String toString() {
        return opcode;
    }

    static int shortFromBuffer(byte[] data, int deb) {
        int res = data[deb];
        res += data[deb + 1] << 8;
        return res;
    }

    public static void parseCommand(ReceiveCommand commande) {
        StringBuilder posx = new StringBuilder();
        StringBuilder posy = new StringBuilder();
        StringBuilder point = new StringBuilder();
        switch (commande.opcode) {
            case "REGOK":
                System.out.println("[GhostLab] > Vous êtes bien enregistré dans la partie : " + commande.data[6]);
                Client.setCurrentClientState(1);
                break;
            case "UNROK":
                System.out.println("[GhostLab] > Vous êtes bien désenregistré de la partie : " + commande.data[6]);
                Client.setCurrentClientState(0);
                break;
            case "GAMES":
                System.out.println("[GhostLab] > Nombre de partie(s) disponible(s) : " + commande.data[6]);
                break;
            case "OGAME":
            case "LIST!":
                System.out.println(
                        "[GhostLab] > Partie " + commande.data[6] + " : " + commande.data[8] + " joueur(s) inscrits");
                break;
            case "SIZE!":
                int line = (shortFromBuffer(commande.data, 8));
                int columns = (shortFromBuffer(commande.data, 11));

                System.out.println(
                        "[GhostLab] > La partie " + commande.data[6] + " a une taille de " + line + "*" + columns);
                break;
            case "START":
                System.out.println("[GhostLab] > Vous êtes en attente que la partie commence");
                break;
            case "REGNO":
                System.out.println("[GhostLab] > Vous n'avez pas été enregistré dans la partie");
                break;
            case "DUNNO":
                System.out.println("[GhostLab] > Vous ne pouvez pas effectuer cette action");
                break;
            case "WELCO":
                Client.setCurrentClientState(2);
                StringBuilder ip = new StringBuilder();
                StringBuilder port = new StringBuilder();
                Client.setOnGame(true);
                Client.setParty(commande.data[6]);
                Client.setLine(shortFromBuffer(commande.data, 8));
                Client.setColumns(shortFromBuffer(commande.data, 11));
                Client.setGhost(commande.data[14]);

                for (int i = 16; i < 30; i++) {
                    ip.append((char) commande.data[i]);
                }

                for (int i = 32; i < 36; i++) {
                    port.append((char) commande.data[i]);
                }

                Client.setIP(ip.toString().split("#")[0]);
                Client.setPort(Integer.parseInt(port.toString()));

                System.out.println("[GhostLab] > Vous êtes bien connecté à la partie : " + Client.getParty());
                System.out.println("[GhostLab] > IP: " + Client.getIP());
                System.out.println("[GhostLab] > Port: " + Client.getPortMulti());
                break;
            case "PLAYR":
                System.out.print("[GhostLab] > Joueur : ");
                for (int i = 6; i < commande.dataLength - 3; i++) {
                    System.out.print(((char) commande.data[i]));
                }
                System.out.println();
                break;
            case "GLIS!":
                System.out.println("[GhostLab] > Il y a " + commande.data[6] + " joueur(s) dans la partie");
                break;
            case "POSIT":
                posx = new StringBuilder();
                posy = new StringBuilder();
                for (int i = 15; i <= 17; i++) {
                    posx.append((char) commande.data[i]);
                }

                for (int i = 19; i <= 21; i++) {
                    posy.append((char) commande.data[i]);
                }
                Client.setX((Integer.parseInt(posx.toString())));
                Client.setY((Integer.parseInt(posy.toString())));
                System.out.println("[GhostLab] > Vous êtes en position (" + Client.getX() + "," + Client.getY() + ")");

                break;
            case "GPLYR":
                posx = new StringBuilder();
                posy = new StringBuilder();
                point = new StringBuilder();

                System.out.print("[GhostLab] > Joueur : ");
                for (int i = 6; i <= 13; i++) {
                    System.out.print(((char) commande.data[i]));
                }
                for (int i = 15; i <= 17; i++) {
                    posx.append(((char) commande.data[i]));
                }
                System.out.print(" x: " + (Integer.parseInt(posx.toString())));

                for (int i = 19; i <= 21; i++) {
                    posy.append(((char) commande.data[i]));
                }
                System.out.print(" y: " + (Integer.parseInt(posy.toString())));

                for (int i = 23; i <= 26; i++) {
                    point.append(((char) commande.data[i]));
                }
                System.out.print(" point: " + (Integer.parseInt(point.toString())));
                System.out.println();
                break;
            case "NSEND":
                System.out.println("[GhostLab] > Vous n'avez pas envoyé de message");
                break;
            case "SEND!":
                System.out.println("[GhostLab] > Le Message a bien été envoyé");
                break;
            case "MALL!":
                System.out.println("[GhostLab] > Le Message a bien été multidiffusé");
                break;
            case "MOVEF":
                posx = new StringBuilder();
                posy = new StringBuilder();
                point = new StringBuilder();
                System.out.print("[GhostLab] > Vous avez croisé un fantôme x : ");
                for (int i = 6; i <= 8; i++) {
                    posx.append(((char) commande.data[i]));
                }
                System.out.print((Integer.parseInt(posx.toString())));
                System.out.print(" y: ");
                for (int i = 10; i <= 12; i++) {
                    posy.append(((char) commande.data[i]));
                }
                System.out.print((Integer.parseInt(posy.toString())));
                System.out.print(" point: ");
                for (int i = 14; i <= 17; i++) {
                    point.append(((char) commande.data[i]));
                }
                System.out.print((Integer.parseInt(point.toString())));
                System.out.println();
                break;
            case "MOVE!":
                posx = new StringBuilder();
                posy = new StringBuilder();
                System.out.print("[GhostLab] > Vous n'avez pas croisé de fantôme x: ");
                for (int i = 6; i <= 8; i++) {
                    posx.append(((char) commande.data[i]));
                }
                System.out.print((Integer.parseInt(posx.toString())));
                System.out.print(" y: ");
                for (int i = 10; i <= 12; i++) {
                    posy.append(((char) commande.data[i]));
                }
                System.out.print((Integer.parseInt(posy.toString())));
                System.out.println();
                break;
            case "GOBYE":
                Client.setCurrentClientState(0);
                System.out.println("[GhostLab] > Le joueur a quitté la partie");
                break;
            case "LEAD!":
                System.out.print("[GhostLab] > Le Leader est : ");
                point = new StringBuilder();

                for (int i = 6; i <= 13; i++) {
                    System.out.print(((char) commande.data[i]));
                }
                for (int i = 15; i <= 18; i++) {
                    point.append(((char) commande.data[i]));
                }
                System.out.println(" avec " + (Integer.parseInt(point.toString()) + " points!"));
                break;
            default:
                break;
        }

    }

    public String getOpcode() {
        return this.opcode;
    }

    public byte[] getBytes() {
        return this.data;
    }

}