#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

  WINDOW * wnd;
  WINDOW * subwnd;
  WINDOW * help;
  WINDOW * help1, * help2, * help3;
  WINDOW * subhelp1, * subhelp2, * subhelp3;
  int fd;

void init_help()
{
  delwin(help);
  delwin(help1);
  delwin(help2);
  delwin(help3);
  delwin(subhelp1);
  delwin(subhelp2);
  delwin(subhelp3);

  help = newwin(3, getmaxx(stdscr), getmaxy(stdscr) - 3, 0);
  wbkgd(help, COLOR_PAIR(1));
  
  help1 = derwin(help, getmaxy(help), getmaxx(help) / 3, 0, 0);
  box(help1, 0, 0);
  subhelp1 = derwin(help1, getmaxy(help1) - 2, getmaxx(help1) - 2, 1, 1);
  wprintw(subhelp1,"F1-open");
  
  help2 = derwin(help, getmaxy(help), getmaxx(help) / 3, 0, getmaxx(help) / 3);
  box(help2, 0, 0);
  subhelp2 = derwin(help2, getmaxy(help2) - 2, getmaxx(help2) - 2, 1, 1);
  wprintw(subhelp2,"F2-save");
  
  help3 = derwin(help, getmaxy(help), getmaxx(help) / 3, 0, getmaxx(help) / 3 * 2);
  box(help3, 0, 0);
  subhelp3 = derwin(help3, getmaxy(help3) - 2, getmaxx(help3) - 2, 1, 1);
  wprintw(subhelp3,"F3-quit");
  
  wrefresh(help);
}

void init_wnd()
{
  delwin(wnd);
  delwin(subwnd);

  wnd = newwin(getmaxy(stdscr) - 3, getmaxx(stdscr), 0, 0);
  wbkgd(wnd, COLOR_PAIR(1));
  box(wnd, 0, 0);

  subwnd = derwin(wnd, getmaxy(wnd) - 4, getmaxx(wnd) - 4, 2, 2);

  wrefresh(wnd);
}

void sig_winch(int signo)
{
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
  resizeterm(size.ws_row, size.ws_col);
  init_help();
  init_wnd();
}

void init()
{
  initscr();
  start_color();
  signal(SIGWINCH, sig_winch);
  cbreak();
  noecho();
  curs_set(0);
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_RED, COLOR_WHITE);
  bkgd(COLOR_PAIR(1));
  refresh();
  init_help();
  init_wnd();
  keypad(subwnd, 1);
}
 
void fin(int ret)
{
  delwin(subhelp1);
  delwin(subhelp2);
  delwin(subhelp3);
  delwin(help1);
  delwin(help2);
  delwin(help3);
  delwin(help);
  delwin(subwnd);
  delwin(wnd);
  endwin();
  close(fd);
  exit(ret);
}

void smail()
{
  int i;
  init_pair(1, COLOR_MAGENTA, COLOR_YELLOW);
  bkgd(COLOR_PAIR(1));
  for(i = 0; i < 10000; i++)
  printw("OOPS!");
  noecho();
  for(i = 0; i < 25; i++)
  getch();
}

void open_file(char * buf, char * b)
{
  int x, y, i;
  char name[50];
  int pos;
  char temp;
  //close(fd);
  wmove(subwnd, 0, 0);
  wprintw(subwnd, "Enter file name: ");
  echo();
  curs_set(1);
  wgetnstr(subwnd, name, 50);
  getyx(subwnd, y, x);
  curs_set(0);
  noecho();
  if ((fd = open(name, O_RDWR)) < 0)
  {
    wattron(subwnd, A_BOLD | COLOR_PAIR(2));
    wmove(subwnd, y + 1, 0);
    wprintw(subwnd, "There was an error opening the file, try again.\n");
    wprintw(subwnd, "The error code is %d.\n", errno);
    wprintw(subwnd, "%s.\n", strerror(errno));
    wattroff(subwnd, A_BOLD);
    wattron(subwnd, COLOR_PAIR(1));
    wprintw(subwnd, "Press any key to continue..");
    wrefresh(subwnd);
    wgetch(subwnd);
    wclear(subwnd);
    * b = 0;
    curs_set(0);
    return;
  }
  wmove(subwnd, y + 2, 0);
  wprintw(subwnd, "File opened successfully\n");
  wprintw(subwnd, "Press any key to continue..");
  wrefresh(subwnd);
  wgetch(subwnd);
  wclear(subwnd);
  wrefresh(subwnd);
  pos = lseek(fd, 0, SEEK_SET);
  wmove(subwnd, 0, 0);
  curs_set(1);
  i = 0;
  while (read(fd, &temp, 1))
  {
    if (i < 4096) 
    {
      buf[i] = temp;
      i++;
    }
  }
  wprintw(subwnd, "%s", buf);
  wrefresh(subwnd);
  * b = 1;
  wmove(subwnd, 0, 0);
}

int main(void)
{
  int i, j;
  int x, y;
  int pos;
  int ch;
  char * buf, * buft;
  char b = 0; //Bool
  init();
  buf = malloc(4096 * sizeof(char));
  for (i = 0; i < 4096; i++)
  {
    buf[i] = ' ';
  }
  while(1)
  {
    ch = wgetch(subwnd);
    switch(ch)
    {
      case KEY_F(1):
      {
        open_file(buf, &b);
        i = 0;
        break;
      }
      case KEY_F(2):
      {
        pos = lseek(fd, 0, SEEK_SET);
        buft = buf;
        for (j = 0; j < 4096; j++)
        {
          write(fd, &buf[j], 1);
          //buft++;
        }
        sleep(3);
        break;
      }
      case KEY_F(3):
      {
        fin(1);
      }
      case KEY_F(4):
      case KEY_F(5):
      case KEY_F(6):
      case KEY_F(7):
      case KEY_F(8):
      case KEY_F(9):
      case KEY_F(10):
      case KEY_F(11):
      case KEY_F(12):
      {
        smail();
        fin(2);
        break;
      }
      default:
      {
        if(b == 0) break;
        switch(ch)
        {
          case KEY_UP:
          {
            getyx(subwnd, y, x);
            wmove(subwnd, y > 0 ? y - 1 : y, x);
            i = i > getmaxx(subwnd) ? i - getmaxx(subwnd) : i;
            break;
          }
          case KEY_LEFT:
          {
            getyx(subwnd, y, x);
            if (x > 0) wmove(subwnd, y, x - 1);
            else if (y > 0) wmove(subwnd, y - 1, getmaxx(subwnd) - 1);
            i = i > 0 ? i - 1 : i;
            break;
          }
          case KEY_DOWN:
          {
            getyx(subwnd, y, x);
            wmove(subwnd, y < (getmaxy(subwnd) - 1) ? y + 1 : y, x);
            i = i < (getmaxy(subwnd) * getmaxx(subwnd)) ? i + getmaxx(subwnd) : i;
            break;
          }
          case KEY_RIGHT:
          {
            getyx(subwnd, y, x);
            if (x < (getmaxx(subwnd) - 1)) wmove(subwnd, y, x + 1);
            else if (y < (getmaxy(subwnd) - 1)) wmove(subwnd, y + 1, 0);
            i = i < (getmaxy(subwnd) * getmaxx(subwnd)) ? i + 1 : i;
            break;
          }
          case KEY_BACKSPACE:
          {
            getyx(subwnd, y, x);
            waddch(subwnd, ' ');
            if (x >= 1)
            {
              wmove(subwnd, y, x - 1);
            }
            else
            {
              if (y >= 1)
              {
                wmove(subwnd, y - 1, getmaxx(subwnd) - 1);
              }
              else
              {
                wmove(subwnd, 0, 0);
              }
            }
            buf[i] = ' ';
            i = i > 0 ? i - 1 : i;
            break;
          }
          default:
          {
            waddch(subwnd, (char)ch);
            wrefresh(subwnd);
            buf[i] = (char)ch;
            i = i < (getmaxy(subwnd) * getmaxx(subwnd)) ? i + 1 : i;
            
          }
        }
      }
    }
  } 
}
