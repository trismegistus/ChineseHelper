#include "CharacterDB.h"
#include "Converter.h"
#include "Vowels.h"
#include "UI.h"

#include <gtk/gtk.h>

int main(int argc, char **argv)
{
    CharacterDB db;
    UI ui(db);

    gtk_init(&argc, &argv);

    ui.create();
    gtk_main();
    return 0;
}
