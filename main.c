
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Plane{
    int width;
    int height;

    char **area; //[][]
}Plane;

typedef struct Point{
    double x;
    double y; 
} Point;

typedef struct Vertices{

    Point point;
    struct Vertices *next;

} Vertices;

void push(Vertices ** head, Point *point) {
    Vertices * new_node;
    new_node = (Vertices *) malloc(sizeof(Vertices));

    new_node->point= *point;
    new_node->next = *head;
    *head = new_node;
}

void print_points(Vertices *list) {
    Vertices *current = list;
    while (current != NULL) {
        printf("Point: (%lf, %lf)\n", current->point.x, current->point.y);
        current = current->next;
    }
}



Plane *New_plane(int width, int height){

    Plane *p = malloc(sizeof(Plane));

    if (!p) {
        perror("Errore allocazione Plane");
        exit(EXIT_FAILURE);
    }

    p->height= height;
    p->width= width;

    /*   
    * [] -> [][][]
    * [] -> [][][]
    * [] -> [][][]
    * [] -> [][][]
    */


    p->area= malloc(height* sizeof(char *));

    if (!p->area) {
        perror("Errore allocazione righe");
        free(p);
        exit(EXIT_FAILURE);
    }


    for (int i= 0; i < height; i++){
        p->area[i] = malloc(width * sizeof(char));

        if (!p->area[i]){
            perror("Errore allocazione colonna");
            for (int j=0;j<i;j++){
                free(p->area[i]);
            }
            free(p->area);
            free(p);
            exit(EXIT_FAILURE);
        }
    }

    // a questo punto area puo essere trattato come un vettore di vettori
    // array bidimensionale

    for (int i=0; i< height; i++){
        for (int j=0;j <width; j++){
            p->area[i][j]= ' ';
        }
    }

    return p;

}

void draw(Plane *p){
    int height=  p->height;
    int width= p->width;

    for (int i= 0; i< height ;i++){
        for (int j=0; j< width ;j++){
            printf("%c ", p->area[i][j]);
        }
        printf("\n");
    }

}

int add_point(Plane *p, int x, int y){
    int pos_x= x ;
    int pos_y= (p->height - y) -1;


    if( (0 <= pos_x && pos_x < p->width) && (0<= pos_y && pos_y < p->height) ){
        p->area[pos_y][pos_x] = '*';
        return 1;
    }else{
        printf("occio: x: %d, y:%d\n", pos_x, pos_y);
    }

    return 0;
}

/*
* y=mx+q
* x=(y-q)/m
*/
int add_line(Plane*p, double slope, double intercept){

    int x=0;
    int y=0;

    while (x< p->width && y < p->height -1){
        int old_y=y;
        y= (slope * x) + intercept;
        if(0<= y && y< p->height){
            if (!add_point(p, x, y)){
                return 1;
            }
        }

        // fallback to make lines thickkkk
        while (abs(old_y -y) > 1){

            if ( old_y < y){
                old_y= old_y+1;
            }else {
                old_y= old_y-1;
            }
            
            int fallback_x= ((old_y) - intercept) / slope;
            
            if ( 0 <= fallback_x && fallback_x < p->width ){
                if(!add_point(p, fallback_x, old_y)){
                    return 1;
                }
            }   
        }
        x++;
    }
    return 0;
}


int add_circle(Plane *p, Point *center, double radius){


    int y_up=0;
    int y_down=0;
    int delta=-1;

    int x=center->x -radius;
    while (x <=center->x +radius){
        int old_y_up=y_up;
        int old_y_down=y_down;

        int old_delta= delta;

        delta = sqrt(pow(radius,2) - pow((x - center->x),2));

        y_up= delta + center->y;
        y_down= center->y -delta;
        
        if((0<= x && x < p->width)){
            if ( (0<= y_up && y_up < p->height )){
                if (!add_point(p, x, y_up)){
                    return 1;
                }
            }

            if ( (0<= y_down && y_down < p->height )){
                if(!add_point(p, x, y_down)){
                    return 1;
                }
            } 
        } 
        
        // fallback for the ho(l)es 
        // todo

        x++;
    }

    return 0;
}

/*
*
*
*/
int _add_vertical_segment(Plane *p,Point *start, Point *finish){
    if(finish->y < start->y){
        _add_vertical_segment(p, finish, start);
    }else{
        for (int y = start->y; y <= finish->y; y++) {
            if (!add_point(p, start->x, y)) {
                return 1; // Errore nell'aggiungere il punto
            }
        }   
    }
    return 0;
}

int add_segment(Plane *p,Point *start, Point *finish){
    
    if (start->x == finish->x && start->y == finish->y){
        if (!add_point(p,start->x, start->y)){
            return 1;
        }
    }else if (start->x == finish->x){
        return _add_vertical_segment(p, start, finish);
    }else if(start->x > finish->x){
        add_segment(p, finish, start);
    }else{
        double slope= (finish->y -start->y) / (finish->x -start->x);

        //printf("%lf", slope);
    
        for (int x = start->x; x <= finish->x; x++) {
            int y = start->y + (slope * (x - start->x));  
            if(!add_point(p, x, y)){
                return 1;
            }  
        }
    }

    return 0;
}

int add_figure(Plane *p, Vertices *vertices){

    Vertices *current = vertices;

    while(current->next != NULL){
        add_segment(p, &current->point, &current->next->point);
        current= current->next;
    }

    return 0;
}


int main(){
    Plane *p = New_plane(40,40);

    Point point_a= {15, 6};
    Point point_b= {20, 6};
    Point point_c= {20, 25};
    Point point_d= {15, 25};

    Point c1={13,3};
    Point c2={22,3};

    Vertices *rectangle= NULL;

    push(&rectangle, &point_a);
    push(&rectangle, &point_b);
    push(&rectangle, &point_c);
    push(&rectangle, &point_d);
    push(&rectangle, &point_a);
 

    //print_points(rectangle);


    //add_point(p, 1, 1);

    //add_line(p, -4, 10);

 

    //add_segment(p, &point_d, &point_a);
    add_figure(p,rectangle);

    draw(p);

}