/*
 *  program:     CRLF Converter
 *  authors:     Gurjit Singh Sandhu and Manjinder Singh Hanjrah
 *  objective:   This is a command line tool, to convert CR-LF text files to LF text files and vice-versa.
 *
 *  Working:     A buffer of length 1024 is maintained in the starting, but grows by a factor of 2, 3, 4, 5 ...n , as required in the program.
 *               The buffer gets almost all of the file content into it with only a li'l bit of modification, which is to replace a CR-LF pair
 *               with only LF, and LF with CR-LF pair, in accordance to the line_terminator specified by the user.
 *               In the end, the entire buffer is written to the file. But there could be many other ways to achieve the same,
 *               one could be writing the modifications to the file, line by line, after reading the old line from the file and then manipulating the
 *               the cursor to the back of the line and then overwrite it and then moving to the next line(ahh.. calculations, they might go wrong).
 *               Beware! If you use this approach you should have enough understanding of what you're doing,
 *               for instance, what happens to a file on writing content to it if we have 'file-mode' as 'a' or 'w' or 'a+'. Also fseek, tellg and other functions
 *               sometimes can almost change the entire file into ugly content, 'coz you might have not read/written things from proper positions in the file.
 * Caution:      It doesn't check, in prior, whether your file is a CR-LF file or LF. If you feed a CR-LF file and set the line_terminator 'CRLF', then you'll be getting
 *               CR-CR-LF in the end of every line of your file. So, a bit of caution here.
 */
#include <stdio.h>
#include <stdlib.h>

#define LINE_TERMINATOR argv[1]
#define FILENAME argv[2]

int main(int argc, char **argv)
{
    /* argv[0] contains the program name,
     * argv[1] is the option to specify the line terminator for the file. [CRLF/LF]
     * argv[2] is the filename
     *
     * And also, every string on argv is null terminated, so we need not worry about it.
     */

    FILE *file;
    file = fopen(FILENAME, "rw+");
    if(file == NULL)
    {
        perror("Error:");
        exit(EXIT_FAILURE);
    }

    int line_terminator = (strcmp(LINE_TERMINATOR, "-CRLF")==0) ?  0 : 1;
    char c;
    int buffer_size = 512;
    char *buffer = malloc(buffer_size * sizeof(char*));
    int parse_count = 0;

    /* For CR-LF */
    if(line_terminator == 0)
    {
        while( (c=fgetc(file)) != EOF )
        {
            if(parse_count > buffer_size)
            {
                buffer_size += 512;
                buffer = realloc(buffer, buffer_size);
                if(buffer == NULL)
                {
                    perror("Error:");
                    exit(EXIT_FAILURE);
                }
            }

            if(c == '\n')
            {
                /* Since, it's CR-LF, we need to write '\r' followed by '\n' to the end of the line.
                 * We also need to check that the parse_count doesn't go out of bound after parse_count++ */
                buffer[parse_count] = '\r';
                parse_count++;
                if( parse_count > buffer_size )
                {
                    buffer_size += 512;
                    buffer = realloc(buffer, buffer_size);
                    if(buffer == NULL)
                    {
                        perror("Error: ");
                        exit(EXIT_FAILURE);
                    }
                }
                buffer[parse_count] = '\n';
            }
        }
    }
    else /* For LF only */
    {
        while( (c=fgetc(file)) != EOF )
        {
            if(parse_count > buffer_size)
            {
                buffer_size += 512;
                buffer = realloc(buffer, buffer_size);
                if(buffer == NULL)
                {
                    perror("Error: ");
                    exit(EXIT_FAILURE);
                }
            }

            /* Since, it's for LF only, we can skip to write this token to the end of the file. */
            if(c == '\r') continue;
            if(c == '\n')
            {
                buffer[parse_count] = '\n';
            }
        }
    }

    /* The entire file has been read now we can safely replace its content with the content available in our buffer. */
    fputs(buffer, file);
    fclose(file);
    return EXIT_SUCCESS;
}
