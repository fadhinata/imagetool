
void kmean_clustering(dlist_t cluster, int k, matrix_t *vectors)
{
  int i, j, *index, n, ndim;
  vector_t *vec, *v; 
  dlist_t *list, *nearlist, *pool;
  dlink_t *link, *a, *b;
  double val, dist;

  assert(vectors);

  index = (int *)malloc(k * sizeof(*index));
  n = matrix_get_rows(vectors);
  ndim = matrix_get_columns(vectors);

  // Selection of initial k-means
  pool = dlist_new();
  for (i = 0; i < n; i++) {
    vec = vector_new(ndim);
    vector_copy_row_vector_of_matrix(vec, vectors, 0, i);
    link = dlink_new();
    link->object = (void *)vec;
    dlist_insert(link, pool);
  }

  // Initial k-cluster by random
  for (i = 0; i < k; i++) {
    // Randomly select a new vector; unselected one
    do {
      index[i] = rand() % dlist_get_count(pool);
      for (j = 0; j < i; j++)
	if (index[i] == index[j]) break;
    } while (j < i);
    // Copy a selected new vector
    a = dlist_glimpse(index[i], pool);
    v = (vector_t *)a->object;
    vec = vector_new_and_copy(v);
    // Assign selecting as centroid
    list = dlist_new();
    list->private = (void *)vec;
    // Insert centroid into k-clustering
    b = dlink_new();
    b->object = (void *)list;
    dlist_insert(b, cluster);
  }

  // Klustering of vector in pool
  while (dlist_get_count(pool) > 0) {
    a = dlist_pop(pool);
    v = (vector_t *)a->object;
    // find the nearest cluster
    for (nearlist = NULL, i = 0; i < k; i++) {
      b = dlist_glimpse(i, cluster);
      list = (dlist_t *)b->object;
      vec = (vector_t *)list->private;
      sub = vector_subtract(v, vec);
      val = vector_dotproduct_vector(v, vec);
      if ((link == NULL) || (val < dist)) {
	dist = val;
	nearlist = list;
      }
    }
    dlist_insert(a, nearlist);
  }

  // Update centroid
  for (i = 0; i < k; i++) {
  }
  // Klustering
  for (i = 0; i < k; i++) {
  }
  // Discriminant

  free(index);
}
