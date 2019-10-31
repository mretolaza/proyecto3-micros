/****************************************
UNIVERSIDAD DEL VALLE DE GUATEMALA

Proyecto 3, analisis de datos
Abril Palencia - 18198
Cristina Bautista - 161260
Maria Mercedes Retolaza - 16339

Descripción: En el siguiente programa se evaluarán los resultados obtenidos por los sensores
de temperatura, presión y humedad.Los datos obtenidos(75000) serán analizados para evaluar
si el punto geográfico de donde se est�n tomando datos es apto / compatible con el productor
de agua natural WARKA.

Los valores con los que se comparar� es con el promedio de temperatura con el cual se recauda agua
a las temperaturas
****************************************/
//librerias a utilizar
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <bits/stdc++.h>

// Tamaño de los arreglos
#define N 87600;

// KERNEL 1, calcular temperatura
__global__ void kernel1(float *b, float promedioT, float promedioH)
{
    float myID = threadIdx.x + blockDim.x * blockIdx.x;
    // Solo trabajan N hilos
    if (myID < N)
    {
        //Calculo del punto de rocio
        b[myID] = (pow((float)(promedioH[myID] / 100), (float)1 / 8) * (112 + (0.9 * promedioT[myID])) + (0.1 * promedioT[myID]) - 112);
    }
}
//------------------------------------------------------------------------------------------
//En esta función se recibe como parámetro un array de numeros y el tamaño
// que tiene dicho array
//Retorna el min valor de los datos ingresdos
//------------------------------------------------------------------------------------------
float minValue(float array[], float size)
{
    float min = array[0];
    for (int i = 0; i < size; i++)
    {
        if (array[i] < min)
        {
            min = array[i];
        }
    }
    return min;
}
//------------------------------------------------------------------------------------------
//En esta función se recibe como parámetro un array de numeros y el tamaño
// que tiene dicho array
//Retorna el max valor de los datos ingresados
//------------------------------------------------------------------------------------------
float maxValue(float array[], float size)
{
    float max = 0;
    for (int i = 0; i < size; i++)
    {
        if (array[i] > max)
        {
            max = array[i];
        }
    }
    return max;
}
//------------------------------------------------------------------------------------------
//En esta función se recibe como parámetro un array de numeros y el tamaño
// que tiene dicho array
//Se calcula la prom de los datos ingresados
//------------------------------------------------------------------------------------------
float averageData(float array[], int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum = sum + array[i];
    }
    float prom = (sum / size);
    return prom;
}

//Main del programa.
float main(float argc, char **argv)
{
    // Lectura de datos
    ifstream leer;
    leer.open("HUM.csv", ios::in);

    //declara los streams para el lanzamiento del kernel
    cudaStream_t stream1;

    //memoria
    float *at, *btodo; // stream 1 mem ptrs
    float *ah;
    float *dev_at, *dev_btodo; // stream 1 mem ptrs
    float *dev_ah;

    //stream 1
    //declaracion de los arreglos en la memoria del device de tamaño N y tipo de datos float.
    cudaMalloc((void **)&dev_at, N * sizeof(float));
    cudaMalloc((void **)&dev_btodo, N * sizeof(float));
    //declaracion de los arreglos en la memoria del HOST de tamaño N y tipo de datos float.
    cudaHostAlloc((void **)&at, N * sizeof(float), cudaHostAllocDefault);
    cudaHostAlloc((void **)&btodo, N * sizeof(float), cudaHostAllocDefault);

    //stream 3
    //declaracion de los arreglos en la memoria del device de tamaño N y tipo de datos float.
    cudaMalloc((void **)&dev_ah, N * sizeof(float));
    //declaracion de los arreglos en la memoria del HOST de tamaño N y tipo de datos float.
    cudaHostAlloc((void **)&ah, N * sizeof(float), cudaHostAllocDefault);

    // llenar los arreglos con los datos del sensor
    string line, dato;
    float fila, columna;
    for (fila = 0; getline(leer, linea); fila++)
    {
        stringstream registro(linea);
        //columna 1: Humedad
        //columna 2; Temperatura
        for (columna = 0; getline(registro, dato, ';'); columna++)
        {
            switch (columna)
            {
            case 0:
                ah[fila] = strtof((dato).c_str(), 0);
                break;
            case 1:
                at[fila] = strtof((dato).c_str(), 0);
                break;
            }
        }
    }

    // Calculo de promedio, maximo y minimo de humedad y temperatura
    cout << "\nEl promedio de los datos es:"
         << "\n";
    float promT = averageData(at, N);
    float promH = averageData(ah, N);
    cout << "Temperatura: " << promT << "\n";
    cout << "Humedad: " << promH << "\n";

    cout << "\nEl maxValue de los datos es:"
         << "\n";
    float maxT = maxValue(at, numLines);
    float maxH = maxValue(ah, numLines);

    cout << "Temperatura: " << maxT << "\n";
    cout << "Humedad: " << maxH << "\n";

    cout << "\nEl minValue de los datos es:"
         << "\n";
    float minT = minValue(at, numLines);
    float minH = minValue(ah, numLines);

    cout << "Temperatura: " << minT << "\n";
    cout << "Humedad: " << minH << "\n";

    //for que lanza los datos a los kernels para ser ejecutados.
    for (float i = 0; i < N; i += N * 2)
    { // loop over data in chunks
        // floaterweave stream 1 and steam 2
        //manda los datos de la memoria del HOST al device
        cudaMemcpyAsync(dev_btodo, btodo, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
        //lanzamiento a los kernels
        //kernel 1
        //float promedioT, float maximoT, float minimoT, float promedioH, float maximoH, float minimoH
        kernel<<<(float)ceil(N / 1024) + 1, 1024, 0, stream1>>>(dev_bt, promT, promH);

        //manda los datos del arreglo c del device al arreglo c del HOST de cada stream
        //stream 1
        cudaMemcpyAsync(btodo, dev_btodo, N * sizeof(float), cudaMemcpyDeviceToHost, stream1);
    }
    float teorico;
    //Calculo del dato teorico de punto de rocio
    teorico = (pow((float)(37.33 / 100), (float)1 / 8) * (112 + (0.9 * 21.6)) + (0.1 * 21.6) - 112);

    //Comparacion para saber si son validos los resultados del sensor
    if (btodo[0] <= teorico)
    {
        cout << "\nEs valido\n";
    }
    else
    {
        cout << "\nNo es valido\n";
    }

    //Destruccion de los streams
    cudaStreamDestroy(stream1);
    return 0;
}
