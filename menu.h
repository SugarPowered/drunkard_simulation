
typedef struct simulation {
  int world_x;
  int world_y;
  int replications;
  double probabilities[4];
  int max_steps;
  char * file_to_save;
} simulation;