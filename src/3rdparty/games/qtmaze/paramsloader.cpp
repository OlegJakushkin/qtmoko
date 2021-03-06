/*  paramsloader.cpp
 *
 *  Config and levelpack loader
 *
 *  (c) 2009-2010 Anton Olkhovik <ant007h@gmail.com>
 *
 *  This file is part of QtMaze (port of Mokomaze) - labyrinth game.
 *
 *  QtMaze is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  QtMaze is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with QtMaze.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "json.h"
#include "types.h"

#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

Config game_config;
Level* game_levels;
User user_set;
Prompt arguments;
int game_levels_count;
int vibro_enabled;
char *exec_init, *exec_final;

#define DATADIR_LOCAL QCoreApplication::applicationDirPath() + "/etc/qtmaze/"
//#define DATADIR_INSTALLED ":/data/"
#ifdef QTOPIA
#define DATADIR_INSTALLED QCoreApplication::applicationDirPath() + "/../etc/qtmaze/"
#else
#define DATADIR_INSTALLED QString("/usr/share/qtmaze/")
#endif

#define SAVE_DIR ".mokomaze"
#define SAVE_FILE "user.json"
char *save_dir_full;
char *save_file_full;
int can_save = 0;


int load_file_to_mem(const char *filename, char **result)
{
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly))
    {
        QTextStream fstream(&f);
        QString s = fstream.readAll();
        QByteArray ba = s.toAscii();
        *result = strdup(ba.data());
        return ba.size();
    }
    return -1;
}

int GetNodesCount(json_t* node)
{
    int co=0;
    while (node)
    {
        co++;
        node = node->next;
    }
    return co;
}

int load_params()
{

    char *pHome = getenv("HOME");
    if (!pHome)
    {
        can_save=0;
        printf("File_loader: can't retrieve environment variable HOME\n");
        printf("File_loader: savegame data not available\n");
    }
    else
    {
        can_save=1;
        save_dir_full = (char*)malloc(strlen(pHome) + strlen("/") + strlen(SAVE_DIR) + 1);
        strcpy(save_dir_full, pHome);
        strcat(save_dir_full, "/");
        strcat(save_dir_full, SAVE_DIR);
        save_file_full = (char*)malloc(strlen(save_dir_full) + strlen("/") + strlen(SAVE_FILE) + 1);
        strcpy(save_file_full, save_dir_full);
        strcat(save_file_full, "/");
        strcat(save_file_full, SAVE_FILE);
    }

    char* cfg;
    char* lvl;
    char* usr = NULL;
    QString config_fname = DATADIR_LOCAL + "config.json";
    if (load_file_to_mem(config_fname.toAscii().data(), &cfg) < 0)
    {
        config_fname = DATADIR_INSTALLED + "config.json";
        if (load_file_to_mem(config_fname.toAscii().data(), &cfg) < 0)
        {
            printf("File_loader: error loading config file\n");
            return -1;
        }
    }
    QString levelpack_fname = DATADIR_LOCAL + "main.levelpack.json";
    if (load_file_to_mem(levelpack_fname.toAscii().data(), &lvl) < 0)
    {
        levelpack_fname = DATADIR_INSTALLED + "main.levelpack.json";
        if (load_file_to_mem(levelpack_fname.toAscii().data(), &lvl) < 0)
        {
            printf("File_loader: error loading levelpack file\n");
            return -1;
        }
    }
    if (can_save)
        if (load_file_to_mem(save_file_full, &usr) < 0)
        {
            printf("File_loader: savegame file not found\n");
        }

    //printf("%s\n",cfg);
    //printf("%s\n",lvl);

    char *document;;
    json_t *root;

//-----------------------------------------------------------
    document = cfg;
    //printf("Parsing the document...\n");
    json_parse_document(&root, document);
    //printf("Printing the document tree...\n");
    //json_render_tree(root);

    //printf("wnd_w\n");
    //printf("%s\n", root->child->child->child->child->text);
    game_config.wnd_w = atoi(root->child->child->child->child->text);
    //printf("wnd_h\n");
    //printf("%s\n", root->child->child->child-> next-> child->text);
    game_config.wnd_h = atoi(root->child->child->child-> next-> child->text);
    game_config.f_delay = atoi(root->child->child->child-> next->next-> child->text);
    game_config.fullscreen = atoi(root->child->child->child-> next->next->next-> child->text);

    //printf("ball_rad\n");
    //printf("%s\n", root->child-> next-> child->child->child->text);
    game_config.ball_r = atoi(root->child-> next-> child->child->child->text);

    //printf("hole_rad\n");
    //printf("%s\n", root->child-> next->next-> child->child->child->text);
    game_config.hole_r = atoi(root->child-> next->next-> child->child->child->text);

    vibro_enabled = atoi(root->child-> next->next->next-> child->child->child->text);

    exec_init  = strdup(root->child-> next->next->next->next-> child->child->child->text);
    exec_final = strdup(root->child-> next->next->next->next-> child->child-> next-> child->text);

    json_free_value(&root);
//-----------------------------------------------------------
//-----------------------------------------------------------
 
    document = lvl;
    //printf("Parsing the document...\n");
    json_parse_document(&root, document);
    //printf("Printing the document tree...\n");
    //json_render_tree(root);

    json_t *level = root->child->   next->next->next->   child->child;
    int levels_count = GetNodesCount(level);
    game_levels_count = levels_count;
    printf("File_loader: %d game levels parsed\n", game_levels_count);
    game_levels = (Level*)malloc(sizeof(Level) * levels_count);
    int i=0;
    int j=0;
    while (level)
    {
        //printf("*************level****************\n");

        if (level->child-> next->next->next->next->next)
        {
            levels_count--;
            game_levels_count = levels_count;
        }
        else
        {
            json_t *box = level->child->  next->  child->child;
            int boxes_count = GetNodesCount(box);
            game_levels[i].boxes_count = boxes_count;
            game_levels[i].boxes = (Box*)malloc(sizeof(Box) * boxes_count);
            j=0;
            while (box)
            {
                game_levels[i].boxes[j].x1 = atoi(box->child->child->text);
                game_levels[i].boxes[j].y1 = atoi(box->child-> next-> child->text);
                game_levels[i].boxes[j].x2 = atoi(box->child-> next->next-> child->text);
                game_levels[i].boxes[j].y2 = atoi(box->child-> next->next->next-> child->text);
                box = box->next;
                j++;
            }

            json_t *hole = level->child-> next->next-> child->child;
            int holes_count = GetNodesCount(hole);
            game_levels[i].holes_count = holes_count;
            game_levels[i].holes = (Point*)malloc(sizeof(Point) * holes_count);
            j=0;
            while (hole)
            {
                game_levels[i].holes[j].x = atoi(hole->child->child->text);
                game_levels[i].holes[j].y = atoi(hole->child-> next-> child->text);
                hole = hole->next;
                j++;
            }

            json_t *fin = level->child-> next->next->next-> child->child;
            int fins_count = GetNodesCount(fin);
            game_levels[i].fins_count = fins_count;
            game_levels[i].fins = (Point*)malloc(sizeof(Point) * fins_count);
            j=0;
            while (fin)
            {
                game_levels[i].fins[j].x = atoi(fin->child->child->text);
                game_levels[i].fins[j].y = atoi(fin->child-> next-> child->text);
                fin = fin->next;
                j++;
            }

            json_t *init = level->child-> next->next->next->next-> child;
            game_levels[i].init.x = atoi(init->child->child->text);
            game_levels[i].init.y = atoi(init->child-> next-> child->text);

            i++;
        }

        level = level->next;
    }
    
    json_free_value(&root);
//==============================================================================

    int savegame_error = 0;
    if (usr)
    {
        document = usr;
        if (json_parse_document(&root, document) != JSON_OK)
        {
            savegame_error = 1;
        }
        else
        {
            json_t *save_section = json_find_first_label(root, "save");
            if ((save_section) && (save_section->child))
            {
                json_t *levelpack_section = json_find_first_label(save_section->child, "levelpack");
                if ((levelpack_section) && (levelpack_section->child) && (levelpack_section->child->text))
                {
                    strcpy(user_set.levelpack, levelpack_section->child->text);
                }
                else
                {
                    savegame_error = 1;
                }

                json_t *level_section = json_find_first_label(save_section->child, "level");
                if ((level_section) && (level_section->child) && (level_section->child->text))
                {
                    user_set.level = atoi(level_section->child->text);
                }
                else
                {
                    savegame_error = 1;
                }
            }
            else
            {
                savegame_error = 1;
            }
            
            // clean up
            json_free_value(&root);
        }
        
        if (savegame_error)
        {
            fprintf(stderr, "File_loader: error parsing savegame file\n");
        }
    }

    if ( (!usr) || (savegame_error) )
    {
        strcpy(user_set.levelpack, "main");
        user_set.level = 1;
    }

    if (cfg) free(cfg);
    if (lvl) free(lvl);
    if (usr) free(usr);

    return 0;
}

#define USERSET_TEMPLATE "{\n\t\"save\": {\n\t\t\"levelpack\": \"main\",\n\t\t\"level\"    : %d\n\t}\n}\n"
void SaveLevel(int n)
{
    if (!can_save) return;

    struct stat st;
    if (stat(save_dir_full, &st) != 0)
    {
        printf("File_saver: directory '%s' not exists. Trying to create.\n", save_dir_full);
        if (mkdir(save_dir_full, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
        {
            printf("File_saver: can't create\n");
            free(save_dir_full); free(save_file_full);
            return;
        }
        printf("File_saver: created successfully\n");
    }

    FILE *f = fopen(save_file_full, "w");
    if (!f)
    {
            printf("File_saver: can't open savegame file '%s' for writing\n", save_file_full);
            free(save_dir_full); free(save_file_full);
            return;
    }
    fprintf(f, USERSET_TEMPLATE, n);
    fclose(f);

    free(save_dir_full); free(save_file_full);
    return;
}

void parse_command_line(int argc, char *argv[])
{
    arguments.level_set = 0;
    arguments.accel_set = 0;
    for (int i=1; i<argc; i++)
    {
        if ( (!strcmp(argv[i],"-l")) || (!strcmp(argv[i],"--level")) )
        {
            if (i+1<argc)
            {
                arguments.level = atoi(argv[i+1]);
                arguments.level_set = 1;
                i++;
            }
            else
            {
                printf("Arguments_parser: level number undefined\n");
                return;
            }
        }
        if ( (!strcmp(argv[i],"-i")) || (!strcmp(argv[i],"--input")) )
        {
            if (i+1<argc)
            {
                int known = 1;
                char *accel_name = argv[i+1];
                if (!strcmp(accel_name, "keyboard")) //not implemented
                {
                    arguments.accel = ACCEL_UNKNOWN;
                }
                else if (!strcmp(accel_name, "freerunner"))
                {
                    arguments.accel = ACCEL_FREERUNNER;
                }
                else
                {
                    printf("Arguments_parser: unknown input device type '%s'\n", accel_name);
                    known = 0;
                    return;
                }
                
                if (known) arguments.accel_set = 1;
                i++;
            }
            else
            {
                printf("Arguments_parser: input device type undefined\n");
                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------

Config GetGameConfig()
{
    return game_config;
}

Level* GetGameLevels()
{
    return game_levels;
}

User GetUserSettings()
{
    return user_set;
}

Prompt GetArguments()
{
    return arguments;
}

int GetGameLevelsCount()
{
    return game_levels_count;
}

int GetVibroEnabled()
{
    return vibro_enabled;
}

char* GetExecInit()
{
    return exec_init;
}
char* GetExecFinal()
{
    return exec_final;
}
