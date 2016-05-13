#ifndef _UI
#define _UI

#include <gtk/gtk.h>
#include "CharacterDB.h"
#include "Converter.h"
#include <list>

class UI
{
    GtkWidget *window;
    GtkWidget *mainVBox;
    GtkWidget *pinyinTable;
    GtkWidget *characterTable;
    GtkWidget *pinyinInput;
    GtkWidget *characterInput;
    GtkWidget *vowelButtons[6];
    CharacterDB& db;
    Converter converter;
    GtkWidget *expandButton;
    std::list<GtkWidget*> characterButtons;

    void createPinyinInput(void);
    void createCharacterInput(void);
    void addVowel(GtkWidget*);
    void displaySuggestions(GtkWidget*);
    void characterFocus(GtkWidget*);
    void appendCharacter(GtkWidget*);
    void expandClicked(GtkWidget*);

    //needed to act as glue between gtk and the acutal handler functions
    static void addVowelGlue(GtkWidget*, gpointer);
    static void displaySuggestionsGlue(GtkWidget*, gpointer);
    static void characterFocusGlue(GtkWidget*, GtkDirectionType, gpointer);
    static void appendCharacterGlue(GtkWidget*, gpointer);
    static void expandClickedGlue(GtkWidget*, gpointer);

    public:
    UI(CharacterDB&);
    ~UI();

    void create(void);
};
#endif
