#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    char **grid;
    char empty, obstacle, full;
    int  *dp;
    int  rows, cols;
    int  max_size, max_rows, max_cols;
} map;

int len(char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return i;
}

int pars_head(FILE *f, map *m)
{
    fscanf(f, "%d", &m->rows);
    if (m->rows <= 0) return 0;
    char *line = 0; size_t l = 0;
    if (getline(&line, &l, f) <= 0) {free(line); return 0;}
    if (len(line) != 4) return 0;
    m->empty = line[0];
    m->obstacle = line[1];
    m->full = line[2];
    free(line);
    return (m->empty != m->obstacle && m->empty != m->full && m->obstacle != m->full);
}


void copy(char *d, char *s, int n)
{
    int i = 0;
    while (i < n)
    {
        d[i] = s[i];
        i ++;
    }
    d[i] = '\n';
}

int load_map(FILE *f, map *m)
{
    char *line = 0; size_t l = 0;
    if (getline(&line, &l, f) <= 0) {free(line); return 0;}
    m->cols = len(line) - 1;
    if (m->cols <= 0) {free(line); return 0;}

    m->grid = malloc(m->rows * sizeof(char *));
    m->dp   = calloc(m->cols * m->rows, 4);

    m->grid[0] = malloc(m->cols + 1);
    copy(m->grid[0], line, m->cols);
    free(line);
    for (int i = 1; i < m->rows; i ++)
    {
        line = NULL; l = 0;
        if (getline(&line, &l, f) <= 0 || (len(line) - 1) != m->cols) {free(line); return 0;}
        m->grid[i] = malloc(m->cols + 1);
        copy(m->grid[i], line, m->cols);
        free(line);
        for (int j = 0; j < m->cols; j ++)
            if (m->grid[i][j] != m->empty && m->grid[i][j] != m->obstacle) return 0;
    }
    return 1;
}

int min3(int min, int a, int b)
{
    if (min > a)
        min = a;
    if (min > b)
        min = b;
    return min;
}

void find_big_squ(map *m)
{
    m->max_cols = 0; m->max_rows = 0; m->max_size = 0;
    for (int i = 0; i < m->rows; i ++)
    {
        for (int j = 0; j < m->cols; j ++)
        {
            if (m->grid[i][j] == m->obstacle)
                m->dp[i * m->cols + j] = 0;
            else
            {
                if (i == 0 || j == 0)
                    m->dp[i * m->cols + j] = 1;
                else
                {
                    m->dp[i * m->cols + j] = min3(m->dp[(i - 1) * m->cols + (j - 1)], m->dp[(i - 1) * m->cols + j], m->dp[i * m->cols + (j - 1)]) + 1;
                }
                if (m->dp[i * m->cols + j] > m->max_size)
                {
                    m->max_size = m->dp[i * m->cols + j];
                    m->max_cols = j - m->max_size + 1;
                    m->max_rows = i - m->max_size + 1;
                }
            }
        }
    }
}

void draw(map *m)
{
    for (int i = 0; i < m->rows; i ++)
    {
        for (int j = 0; j < m->cols; j ++)
        {
            if (i >= m->max_rows && i < m->max_rows + m->max_size &&
                j >= m->max_cols && j < m->max_cols + m->max_size)
                fprintf(stdout, "%c", m->full);
            else
                fprintf(stdout, "%c", m->grid[i][j]);
        }
        fprintf(stdout, "\n");
    }
}

void cleanup(map *m)
{
    if (m->grid)
    {
        for (int i = 0; i < m->rows; i ++) free(m->grid[i]);
        free(m->grid);
    }
    if (m->dp)
        free(m->dp);
}

void proccess_map(FILE *f)
{
    map m = {0};
    if (!pars_head(f, &m) || !load_map(f, &m))
    {
        fprintf(stderr, "map error\n");
        return;
    }
    find_big_squ(&m);
    draw(&m);
    cleanup(&m);
}


int main(int ac, char** av)
{
    if (ac == 1)
        proccess_map(stdin);
    else
    {
        for (int i = 1; i < ac; i ++)
        {
            FILE *f = fopen(av[i], "r");
            if (!f)
            {
                fprintf(stderr, "map error\n");
                continue;
            }
            proccess_map(f);
            fclose(f);
        }
    }
    return 0;
}