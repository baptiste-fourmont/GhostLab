import java.util.ArrayList;

public enum Actions {
    UNAME("PSEUD", "Changer de pseudo"),
    NEWPL("NEWPL", "Créer une nouvelle partie"),
    REGIS("REGIS", "Se connecter à une partie"),
    START("START", "Se mettre prêt à jouer"),
    UNREG("UNREG", "Se déconnecter de la partie"),
    SIZEX("SIZE?", "Taille de la grille d'une partie"),
    LISTX("LIST?", "Liste des joueurs inscrits dans une partie"),
    GAMEX("GAME?", "Nombre de parties non-commencées"),
    UPMOV("UPMOV", "Se déplacer vers le haut"),
    DOMOV("DOMOV", "Se déplacer vers le bas"),
    LEMOV("LEMOV", "Se déplacer vers la gauche"),
    RIMOV("RIMOV", "Se déplacer vers la droite"),
    IQUIT("IQUIT", "Quitter la partie"),
    GLISX("GLIS?", "Voir les joueurs présents"),
    SENDX("SEND?", "Envoyer un message à un joueur"),
    MALLX("MALL?", "Envoyer un message à tous les joueurs"),
    LEAD("LEAD?", "Demander le leader");


    private String opcode, description;

    Actions(String code, String desc) {
        this.opcode = code;
        this.description = desc;
    }

    public static void printActionList(ArrayList<Actions> actions, boolean withDesc){
        for (int i = 0; i < actions.size(); i++) {
            Actions action = actions.get(i);
            if (withDesc) {
                System.out.println("[" + i + "] " + action.getOpcode() + " : " + action.getDescription());
            } else {
                System.out.print("[" + i + "] " + action.getOpcode() + "  |  ");
            }
        }
        if(!withDesc){
            System.out.println();
        }
    }

    public static ArrayList<Actions> addActionToList(int currentState){
        ArrayList<Actions> actions = new ArrayList<>();
        switch (currentState) {
            case 0: {
                actions.add(Actions.NEWPL);
                actions.add(Actions.REGIS);
                actions.add(Actions.GAMEX);
                actions.add(Actions.SIZEX);
                actions.add(Actions.LISTX);
                break;
            }
            case 1: {
                actions.add(Actions.START);
                actions.add(Actions.UNREG);
                actions.add(Actions.SIZEX);
                actions.add(Actions.LISTX);
                actions.add(Actions.GAMEX);
                break;
            }
            case 2: {
                actions.add(Actions.UPMOV);
                actions.add(Actions.DOMOV);
                actions.add(Actions.LEMOV);
                actions.add(Actions.RIMOV);
                actions.add(Actions.IQUIT);
                actions.add(Actions.GLISX);
                actions.add(Actions.MALLX);
                actions.add(Actions.SENDX);
                actions.add(Actions.LEAD);
                break;
            }
        }
        return actions;
    }

    public String getDescription() {
        return this.description;
    }

    public String getOpcode() {
        return this.opcode;
    }
}
