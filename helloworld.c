#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


int main(void)
{
  int fd; //File descriptor
  int err; //Error id
  int pos; //File position
  char ch; //Readable character
  if ((fd = open("helloworld.txt", O_RDWR)) < 0)
  {
    printf("Error(%d)..\n", &fd);
    return -1;
  }
  else
  {
    printf("File 'heloworld.txt' create and open\n");
    if ((err = write(fd, "Hello World!", 12)) < 0)
    {
      printf("Error(%d)..\n", &err);
      return -1;
    }
    else
    {
      fsync(fd);
      printf("The file 'helloworld.txt' was recorded\n");
      pos = lseek(fd, -1, SEEK_END);
      while (pos >= 0)
      {
        pos = lseek(fd, pos, SEEK_SET);
        if ((err = read(fd, &ch, 1)) < 0)
        {
          printf("Error(%d)..\n", &err);
          return -1;
        }
        else
        {
          printf("%c", ch);
          pos = pos - 1;
        }
      }
    }
    printf("\n");
    close(fd);
    return 0;
  }
}
