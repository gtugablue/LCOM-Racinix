/*
 Menu principal do programa, a partir de qual sao chamadas as seguintes funcoes
 da particao "video_gr.c".
 Atraves do menu principal pode fazer os testes de qualquer uma das funcoes.
 */

#include <minix/drivers.h>

#include "video_gr.h"


static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char *argv[]);
#define WAIT_TIME 5

int main(int argc, char **argv) {
long cor;
vg_init(0x105);
vg_draw_line(1,200,400,0,9);
/*vg_fill(9);
vg_set_pixel(40,5,6); */
cor = vg_get_pixel(40,5);
sleep (WAIT_TIME);
vg_exit();

/*printf("%d", cor);*/
}
