//
//  main.c
//  6010A2
//
//  Created by thuzmh on 9/13/17.
//  Copyright © 2017 thuzmh. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define THRESHOLD   2
#define MAX_REPEAT  50

void usage(const char *programName);
float **malloc_mat(int n1, int n2);
void free_mat(int n1, int n2, float **a);
void update_cluster(int num_items, int num_attrs, int k,\
                    int *cluster_num ,float **sample,float **mu);
void print_cluster(int num_items, int num_attrs, int k,\
                   int *cluster_num ,float **sample,float **mu);

void update_mu(int num_items, int num_attrs, int k,\
               int *cluster_num ,float **sample,float **mu);
float distance(float *a, float *b, int num_attrs);

float sum_sqare_dis(int num_items, int num_attrs, int k,\
                    int *cluster_num ,float **sample,float **mu);

FILE * dataIn; //declare the file pointer


//the first argument: file name
//the second argument: input data_file name
//the third argument: k
//the last argument: output file_name

int main(int argc, const char * argv[]) {
    
/*Do the prepare work from line 30 - line 67*/
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }
    int i,j = 0; //looping vars
    //get the name of input_date, k and output_data
    const char * input_data = argv[1];
    int k = (int) strtol(argv[2], NULL, 10);
    const char * output_data = argv[3];
    printf("Hello, World!\n %s %d %s\n",input_data,k,output_data);
    if(k < 1){printf("Wrong k!\n"); return 0;};
    
    //get the num_items and num_attrs
    int num_items = 0,num_attrs = 0;
    //load data from the file
    dataIn = fopen (input_data, "rt"); // open the file for reading
    if(dataIn == NULL){
        printf("File Read Error");
        return 0;
    }
    fscanf(dataIn, "%d" , &num_items);
    fscanf(dataIn, "%d" , &num_attrs);//assign the first line from file
    printf("%d %d\n", num_items, num_attrs);
    float **sample = malloc_mat(num_items, num_attrs); // malloc sample data
    
    for (i = 0 ; i < num_items ; i ++)
        for (j = 0; j < num_attrs; j++){
            if(!feof(dataIn)){
                fscanf(dataIn, "%f", &sample[i][j]);
                printf("%d %d : %f\n",i,j,sample[i][j]);
            }
            else {printf("Num does not match1!\n"); return 0;}
        }
        if(i!=num_items || j!= num_attrs){
        printf("Num does not match2!\n");
        return 0;
    }
    
    //create centroid mu
    float **mu = malloc_mat(k, num_attrs);
    //initiate mu
    for (i = 0; i < k; i++)
        for(j = 0; j< num_attrs; j++){
            mu[i][j] = sample[i][j];
            printf("mu[%d][%d] is %f\n",i,j,mu[i][j]);
        }
    
/*
 From now on, the K-means Algorithm begins !
 the parameter we have: k && num_items && num_attrs && sample[num_i][num_a] && mu(initiated)
 
     
*/
    int B = 0;
    int * cluster_num = (int*) malloc (num_items*sizeof(int));
    //initialize cluster_num:
    for(i = 0; i < num_items; i++)
        cluster_num[i] = 0;
    update_cluster(num_items, num_attrs, k, cluster_num, sample, mu);
    float A = sum_sqare_dis(num_items, num_attrs, k, cluster_num ,sample, mu);
    printf("A is %f",A);
    while(A>THRESHOLD && B<MAX_REPEAT){
        update_cluster(num_items, num_attrs, k, cluster_num, sample, mu);
        print_cluster(num_items, num_attrs, k, cluster_num, sample, mu);
        update_mu(num_items, num_attrs, k, cluster_num ,sample, mu);

        A = sum_sqare_dis(num_items, num_attrs, k, cluster_num ,sample, mu);
        printf("A is %f\n",A);
        
        B = B+1;
    }
    if (A<THRESHOLD)
        printf("Succeed!!\n");
    else
        printf("Completes some maximum number of iterations!\n");
    
    free_mat(num_items, num_attrs, sample);
    free_mat(k , num_attrs , mu);
    return 0;
}






/*
 The Below is some sub functions
 */

void usage(const char *programName) {
    printf("usage: %s  input_data file output_data\n", programName);
    printf("where:\n");
    printf("    input_data    : input file's name\n");
    printf("    k             : number of clusters\n");
    printf("    output_data   : output file's name\n");
    printf("\nExample: %s test0.txt 2 output_data0.txt\n", programName);
}

// initiate the sample_container
float **malloc_mat(int n1, int n2) {
    
    float **mat = NULL;       // pointer to the matrix
    int i;                 // index of mat
    if(n1>0 && n2>0){
        mat = (float **)malloc (n1 * sizeof (float *));
        for(i=0 ; i<n1 ; i++)
            mat[i] = (float *)malloc (n2 * sizeof (float));
    }
    
    else{
        printf("You cannot set n1 or n2 to be negative!! Try again");
        exit(EXIT_FAILURE);
    }    // - check for invalid inputs!
    
    // - update the 'mat' variable
    return mat;
}

//free thememory
void free_mat(int n1, int n2, float **a) {
    
    int i;                                //index of mat
    if(n1>0 && n2>0 && a!= NULL){
        for(i = 0 ; i < n1; i++ ){
            free(a[i]);
        }
    }
    else{
        printf("You cannot set n1 or n2 to be negative!! Try again");
        exit(EXIT_FAILURE);
    }
    // - check for invalid inputs!
}

//caculate the distance between two nodes
float distance(float *a, float *b, int num_attrs){
    int i = 0;
    float dis = 0;
    //printf("fuck\n");
    for(i=0 ; i< num_attrs; i++){
        dis = dis+ (a[i]-b[i])*(a[i]-b[i]);
        //printf("distance is: %f\n",dis);
    }
    return dis;
}

//cluster number:
//From 1 to k, if numeber is 0, the number is wrong.
void update_cluster(int num_items, int num_attrs, int k,\
                    int *cluster_num ,float **sample,float **mu){
    int i,j;
    distance(sample[1], mu[0], num_attrs);
    float min;
    for(i = 0; i < num_items; i++)
    {
        cluster_num[i] = 1 ;
        min = distance(sample[i], mu[0], num_attrs);
        for(j = 1 ; j < k; j ++){
            if(distance(sample[i], mu[j], num_attrs)<min){
                min =distance(sample[i], mu[j], num_attrs);
                cluster_num[i] = j+1;
            }
        }
    }
}

//print each sample with its current cluster ID
void print_cluster(int num_items, int num_attrs, int k,\
                   int *cluster_num ,float **sample,float **mu){
    int i,j;
    for(i = 0; i < num_items; i++){
        printf("Now the sample's locaton is at: ");
        for(j = 0; j< num_attrs; j++)
            printf("%f ",sample[i][j]);
        printf("\nsample_number %d: belongs to cluster: %d\n",i,cluster_num[i]);
        printf("And the cluster's mu is:");
        for(j = 0; j< num_attrs; j++)
            printf("%f ",mu[cluster_num[i]-1][j]);
        printf("\n");
    }
    printf("End of this iteration.\n");
}
//update mu to its members' gravity centroid
void update_mu(int num_items, int num_attrs, int k,\
               int *cluster_num ,float **sample,float **mu){
    int i,j,m;
    float weight_count,num_count;
    for(i = 0 ; i < k; i++){
        weight_count = 0;
        num_count = 0;
        for(j = 0; j < num_attrs; j++){
            for(m = 0 ; m < num_items; m++)
                if(cluster_num[m]==i+1){
                    weight_count = weight_count + sample[m][j];
                    num_count = num_count + 1;
                }
            mu[i][j] = weight_count/num_count;
        }
    }
}

float sum_sqare_dis(int num_items, int num_attrs, int k,\
                    int *cluster_num ,float **sample,float **mu){
    float ans=0;
    int i;
    for(i = 0; i < num_items ; i ++ ){
        ans = ans + distance(sample[i], mu[cluster_num[i]-1], num_attrs);
    }
    ans = ans / num_items;
    ans = sqrt(ans);
    return ans;
}








