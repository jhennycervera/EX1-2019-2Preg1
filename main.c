/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Jhenny Cervera Vargas(20182841)
 *Parcial 2019-2
 * Created on 25 de abril de 2020, 10:56
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_LIN 80
/*
 * 
 */

void ImprimirLin(FILE* archPronVent,int cant,char c){
    for(int i=0; i<cant; i++){
        fprintf(archPronVent, "%c", &c);
    }
    fprintf(archPronVent, "\n");
}

void ObtenerCapProduccion(int aa,int cod_prod,int *cantmax, int *cantmin){
    FILE*archProd;
    archProd=fopen("Capacidad_Produccion.txt", "r");
    if(archProd==NULL){
        printf("ERROR: No se pudo abrir el archivo Capacidad_Produccion.txt\n");
        exit(1);
    }
    int a, cod, min, max;
    while(1){
        fscanf(archProd, "%d %d %d %d", &a, &cod, &min, &max);
        if(feof(archProd)) break;
        if(a==aa && cod_prod==cod){
            *cantmax=max;
            *cantmin=min;
            break;
        }
    }
    fclose(archProd);
    
}
void  ObtenerStocks(int *stocki,int aa,int *stock_seg,int cod_prod){
   
    FILE* archAlm;
    archAlm=fopen("Proyeccion_almacen.txt", "r");
    if(archAlm==NULL){
        printf("ERROR, no se pudo leer el archivo Proyeccion_almacen.txt\n");
        exit(1);
    }
    
    int a, codprod, s_seg, s_proy;
    while(1){
        fscanf(archAlm,"%d %d %d %d",&a, &codprod, &s_seg, &s_proy );
        if(feof(archAlm)) break;
        if(a==aa && codprod==cod_prod){
            *stock_seg=s_seg;
            *stocki=s_proy;
            break;
        }
    }
    fclose(archAlm);
}
int CalcularProd(int proy_vent ,int stock_seg,int aa,int cod_prod, int stocki, int * st_dis, int *pend_prod, int*cap_disp){
    int cantmax, cantmin, produccion;
    ObtenerCapProduccion(aa,cod_prod,&cantmax, &cantmin);
    
    int faltaProd, bloques;
    faltaProd= proy_vent -(stocki-stock_seg);
    bloques=ceil(faltaProd/(double)cantmin);
    produccion=bloques*cantmin;
    if(produccion>cantmax){
        produccion=cantmax;
        if(faltaProd>cantmax){
            *pend_prod=faltaProd-cantmax;
            *cap_disp=0;
        }
        else{ //si alcanza lo que se requiere, pero la produccion es mayor al cantmax
            *pend_prod=0;
            *cap_disp=cantmax-faltaProd;
        }
    }
    
    else *pend_prod=0;
    *st_dis=produccion-faltaProd;
    *cap_disp=cantmax-produccion;
    return produccion;
    
        
}
void ImprimirRep(int anho,int aa,FILE*archPronVent,FILE*archRep, int*prod_anual,int cod_prod){
    
    int sem, disp_vent;
    int stocki,u_prod, stock_seg, st_dis, pend_prod, cap_disp;
    char c;
    ObtenerStocks(&stocki,aa,&stock_seg,cod_prod);
    
    
    while(1){
        fscanf(archPronVent,"%d %d", &sem, &disp_vent);
        if(anho==aa){ // verifico porq voy a procesar datos solo si es del año
            u_prod= CalcularProd(disp_vent,stock_seg,aa,cod_prod, stocki,&st_dis,&pend_prod, &cap_disp);
            if(st_dis<0) st_dis=0;
            fprintf(archRep, "%3d  %11d  %14d  %15d  %11d  %11d  %11d  %11d\n",sem-1, stocki,u_prod,disp_vent, stock_seg, st_dis, pend_prod, cap_disp);
            (*prod_anual)+=u_prod;
            stocki=stock_seg+st_dis;
        }
        
        fscanf(archPronVent,"%c",&c);
        if(c=='\n') break;
        
    }
    ImprimirLin(archRep,MAX_LIN,'=');
}

int main(int argc, char** argv) {

    int aa;
    scanf("%d", &aa);
    
    FILE*archPronVent;
    archPronVent=fopen("Pronostico_ventas.txt", "r");
    if(archPronVent==NULL){
        printf("Error, no s epudo abrir el archivo Pronostico_ventas.txt\n");
        exit(1);
    }
    FILE*archRep;
    archRep=fopen("reportee.txt", "w");
    if(archRep==NULL){
        printf("Error, no s epudo abrir el archivo Pronostico_ventas.txt\n");
        exit(1);
    }
    
    //cabecera
    fprintf(archRep,"%10s","PLAN DE PRODUCCION\n");
    fprintf(archRep, "%15d\n", &aa);
    ImprimirLin(archRep,MAX_LIN,'=');
    
    int anho, cod_prod,sem, cant_venta, prodMayor,cant_ventaMayor=0, prod_Mayor2,cant_ventaMayor2=0;
    int prod_anual=0;
    while(1){
        fscanf(archPronVent,"%d %d", &anho, &cod_prod);
        if(feof(archPronVent)) break;
        if(anho==aa){
            fprintf(archRep, "Producto: %d\n",cod_prod );
            ImprimirLin(archRep,MAX_LIN,'-');
            fprintf(archRep, "Semana Stock Inicial  Unidades Producidas  Disp.Ventas  St5.Seguridad  St.Disponible  Pend.Prod  Cap.Disp\n");
            ImprimirLin(archRep,MAX_LIN,'-');
        
        }
        prod_anual=0;
        ImprimirRep(anho,aa,archPronVent,archRep, &prod_anual,cod_prod); //imprimo si o si, aunq no sea del año, pues tengo que leer los datos
                                                                          // o hacer else, while(fgetc !="\n")
        
        if(anho==aa){
            if(prod_anual>=cant_ventaMayor){
                cant_ventaMayor2=cant_ventaMayor;
                prod_Mayor2=prodMayor;
                
                cant_ventaMayor=prod_anual;
                prodMayor=cod_prod;
            }
            else{
                if(prod_anual>cant_ventaMayor2){
                cant_ventaMayor2=prod_anual;
                prod_Mayor2=cod_prod;
                }
            }
        }
    }
    fprintf(archRep, "Resumen:\n");
    fprintf(archRep, "Producto con mayor produccion en %d: %d   %d\n", aa, prodMayor, cant_ventaMayor);
    fprintf(archRep, "Producto con la segunda mayor produccion en %d: %d    %d\n", aa, prod_Mayor2, cant_ventaMayor2);
    ImprimirLin(archRep,MAX_LIN,'=');
    fclose(archPronVent);
    fclose(archRep);
    
    return (EXIT_SUCCESS);
}

