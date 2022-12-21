#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void copy_file(const char *src_path, const char *dst_path) {
  FILE *src_file = fopen(src_path, "rb");
  FILE *dst_file = fopen(dst_path, "wb");

  if (src_file == NULL || dst_file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  char buffer[1024];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
    fwrite(buffer, 1, bytes_read, dst_file);
  }

  fclose(src_file);
  fclose(dst_file);
}

void copy_directory(const char *src_path, const char *dst_path) {
  DIR *dir = opendir(src_path);
  if (dir == NULL) {
    perror("Error opening directory");
    exit(EXIT_FAILURE);
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char src_path_full[1024];
    snprintf(src_path_full, sizeof(src_path_full), "%s/%s", src_path, entry->d_name);

    char dst_path_full[1024];
    snprintf(dst_path_full, sizeof(dst_path_full), "%s/%s", dst_path, entry->d_name);

    struct stat sb;
    if (stat(src_path_full, &sb) == -1) {
      perror("Error getting file info");
      exit(EXIT_FAILURE);
    }

    if (S_ISDIR(sb.st_mode)) {
      copy_directory(src_path_full, dst_path_full);
    } else {
      copy_file(src_path_full, dst_path_full);
    }
  }

  closedir(dir);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s SOURCE DESTINATION\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  copy_directory(argv[1], argv[2]);
  return 0;
}