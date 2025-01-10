
typedef struct simulation {
  int world_x;
  int world_y;
  int replications;
  double probabilities[4];
  int max_steps;
  int interactive_world;
  int obstacle_world;
  char * file_to_save;
  char * file_to_load;
} simulation;