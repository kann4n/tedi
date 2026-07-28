#ifndef UTILS_H
#define UTILS_H

/*
 * Extracts the filename from a given path.
 *
 * \param destination buffer to store the filename.
 * \param source path from which to extract the filename.
 * \param maximum length of the filename to extract.
 * \return length of the extracted filename.
 *
 * note: path must be in the unix format.
 */
int getfilename(char *dst, const char *src, int maxlen);


/*
 * Sets the foreground color of the terminal.
 *
 * \param buffer to store the color code.
 * \param red component of the color.
 * \param green component of the color.
 * \param blue component of the color.
 */
void setColorFG(char *fg, char r, char g, char b);

/*
 * Sets the background color of the terminal.
 *
 * \param buffer to store the color code.
 * \param red component of the color.
 * \param green component of the color.
 * \param blue component of the color.
 */
void setColorBG(char *bg, char r, char g, char b);

#endif
