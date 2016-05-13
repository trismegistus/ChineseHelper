#include "UI.h"
#include <functional>
#include "Vowels.h"
#include "Converter.h"
#include <ctype.h>
#include <iostream>

UI::UI(CharacterDB& db) : db(db)
{
}

UI::~UI()
{
}

//create the area with the pinyin input
void UI::createPinyinInput(void)
{
    this->pinyinTable = gtk_table_new(1, 5, true);
    for(int count = 0; count < 5; count++)
    {
        //create the five vowel buttons and set them up
        this->vowelButtons[count] = gtk_button_new();
        gtk_table_attach_defaults(GTK_TABLE(this->pinyinTable), this->vowelButtons[count], 
        count, count + 1, 
        0, 1);
        g_signal_connect(this->vowelButtons[count], "clicked", G_CALLBACK(UI::addVowelGlue), this);
        
    }

    //set the default vowel values
    gtk_button_set_label(GTK_BUTTON(this->vowelButtons[0]), "a");
    gtk_button_set_label(GTK_BUTTON(this->vowelButtons[1]), "e");
    gtk_button_set_label(GTK_BUTTON(this->vowelButtons[2]), "i");
    gtk_button_set_label(GTK_BUTTON(this->vowelButtons[3]), "o");
    gtk_button_set_label(GTK_BUTTON(this->vowelButtons[4]), "\xC3\xBC");

    //create the textbox
    this->pinyinInput = gtk_entry_new();

    //handler for when the text changes
    g_signal_connect(this->pinyinInput, "changed", G_CALLBACK(UI::displaySuggestionsGlue), this);

    //pack the widgets
    gtk_box_pack_start(GTK_BOX(this->mainVBox), this->pinyinInput, true, true, 5);
    gtk_box_pack_start(GTK_BOX(this->mainVBox), this->pinyinTable, true, true, 5);
}

//create the area with the character buttons
void UI::createCharacterInput(void)
{
    this->characterTable = gtk_table_new(1, 6, true);
    this->expandButton = gtk_button_new_with_label("\xE2\x87\x93"); //⇓
    gtk_table_attach_defaults(GTK_TABLE(this->characterTable), this->expandButton, 
    5, 6, 
    0, 1);

    //handler for when the expand button is clicked
    g_signal_connect(this->expandButton, "clicked", G_CALLBACK(UI::expandClickedGlue), this);

    this->characterInput = gtk_entry_new();
    //handler for when the character textbox recieves focus
    g_signal_connect(this->characterInput, "focus-in-event", G_CALLBACK(UI::characterFocusGlue), this);

    //pack everything
    gtk_box_pack_start(GTK_BOX(this->mainVBox), this->characterInput, true, true, 5);
    gtk_box_pack_start(GTK_BOX(this->mainVBox), this->characterTable, true, true, 5);
}

void UI::addVowel(GtkWidget *button)
{
    std::string contents = gtk_entry_get_text(GTK_ENTRY(this->pinyinInput));
    std::string vowel = gtk_button_get_label(GTK_BUTTON(button));

    //if the last character is a vowel, replace it with its selection substitution
    //otherwise, append the selected vowel
    if(isVowel(contents[contents.length() - 1]))
    {
        contents.pop_back();
        contents += vowel;
    }
    else
    {
        contents += vowel;
    }

    gtk_entry_set_text(GTK_ENTRY(this->pinyinInput), contents.c_str());
}

//displays vowel suggestions
void UI::displaySuggestions(GtkWidget *textbox)
{
    std::string contents = gtk_entry_get_text(GTK_ENTRY(this->pinyinInput));
    char end = contents[contents.length() - 1];

    if(isVowel(end))
    {
        //get the suggestions and change the buttons to display the vowels
        const std::string *suggestions = vowelSuggestions(contents[contents.length() - 1]);
        int count = 0;

        for(int count = 0; count < 5; count++)
        {
            gtk_button_set_label(GTK_BUTTON(this->vowelButtons[count]), suggestions[count].c_str());
        }
    }
    else
    if(isdigit(end))
    {
        //if a number was hit, append the corresponding vowel to the textbox
        int digit = end - '0';

        contents.pop_back();
        if(isVowel(contents[contents.length() - 1]) && digit > 0 && digit < 6)
        {
            contents.pop_back();
            contents += gtk_button_get_label(GTK_BUTTON(this->vowelButtons[digit - 1]));
        }
        gtk_entry_set_text(GTK_ENTRY(textbox), contents.c_str());
    }
}

//when the character textbox gains focus, display the character with the inputed reading
void UI::characterFocus(GtkWidget *input)
{
    std::string reading = gtk_entry_get_text(GTK_ENTRY(this->pinyinInput));
    std::vector<uint32_t> characters = db.lookupSortedCharacters(reading);
    int size = (characters.size() > 5) ? 5 : characters.size(); //display five characters to begin with

    //not the most efficient, but might as well start with a clean slate
    for(GtkWidget *widget : this->characterButtons)
    {
        gtk_widget_destroy(widget);
    }
    this->characterButtons.clear();

    //create the character buttons and show them
    for(int count = 0; count < size; count++)
    {
        GtkWidget *temp = gtk_button_new_with_label(this->converter.toUTF8(characters[count]).c_str());
        gtk_table_attach_defaults(GTK_TABLE(this->characterTable), temp, count, count + 1, 0, 1);
        gtk_widget_show(temp);
        g_signal_connect(temp, "clicked", G_CALLBACK(UI::appendCharacterGlue), this);
        this->characterButtons.push_back(temp);
    }
}

//adds a character to the character textbox
void UI::appendCharacter(GtkWidget *button)
{
    std::string contents = gtk_entry_get_text(GTK_ENTRY(this->characterInput));
    contents += gtk_button_get_label(GTK_BUTTON(button));
    gtk_entry_set_text(GTK_ENTRY(this->characterInput), contents.c_str());
}

//called when the expand/contract button is clicked
void UI::expandClicked(GtkWidget *button)
{
    std::string status = gtk_button_get_label(GTK_BUTTON(button));

    this->characterFocus(this->pinyinInput);
    if(status == "\xE2\x87\x93") //⇓
    {
        //if we can expand
        std::vector<uint32_t> suggestions = this->db.lookupSortedCharacters(std::string(gtk_entry_get_text(GTK_ENTRY(this->pinyinInput))));

        gtk_button_set_label(GTK_BUTTON(button), "\xE2\x87\x91"); //⇑
        if(suggestions.size() > 5)
        {
            //make enough room for all the characters
            gtk_table_resize(GTK_TABLE(this->characterTable), (suggestions.size() - 5) % 6 + 1, 6);

            //create the additional buttons
            for(int count = 0; count < suggestions.size() - 5; count++)
            {
                GtkWidget *temp = gtk_button_new_with_label(this->converter.toUTF8(suggestions[count + 5]).c_str());
                gtk_table_attach_defaults(GTK_TABLE(this->characterTable), temp, count % 5, count % 5 + 1, count / 5, count / 5 + 1);
                this->characterButtons.push_back(temp);
                g_signal_connect(temp, "clicked", G_CALLBACK(UI::appendCharacterGlue), this);
                gtk_widget_show(temp);
            }
        }
    }
    else
    {
        //otherwise, contract
        //this may be a memory leak, depending on what happens to widgets when a table is resized

        gtk_button_set_label(GTK_BUTTON(button), "\xE2\x87\x93"); //⇓
        gtk_table_resize(GTK_TABLE(this->characterTable), 1, 6);
    }
}

void UI::addVowelGlue(GtkWidget *button, gpointer data)
{
    static_cast<UI*>(data)->addVowel(button);
}

void UI::displaySuggestionsGlue(GtkWidget *entry, gpointer data)
{
    static_cast<UI*>(data)->displaySuggestions(entry);
}

void UI::characterFocusGlue(GtkWidget *widget, GtkDirectionType dir, gpointer data)
{
    static_cast<UI*>(data)->characterFocus(widget);
}

void UI::appendCharacterGlue(GtkWidget *button, gpointer data)
{
    static_cast<UI*>(data)->appendCharacter(button);
}

void UI::expandClickedGlue(GtkWidget *button, gpointer data)
{
    static_cast<UI*>(data)->expandClicked(button);
}

void UI::create(void)
{
    this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    this->mainVBox = gtk_vbox_new(false, 10);

    this->createPinyinInput();
    this->createCharacterInput();

    gtk_container_add(GTK_CONTAINER(this->window), this->mainVBox);
    gtk_widget_show_all(this->window);
}
