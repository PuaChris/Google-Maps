#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

void collect_intersections();
int autocomplete();
char** command_completion(const char* stem_text, int start, int end);
char* intersection_name_generator(const char* stem_text, int state);

#endif /* AUTOCOMPLETE_H */

