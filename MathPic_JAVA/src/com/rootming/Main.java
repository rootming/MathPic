package com.rootming;


import java.io.*;

public class Main {
    static int dim = 1920 * 3;
    public static void pixelWrite(Arg unit){
        double a = 0, b = 0, c = 0, d = 0, n = 0;
        int pos = 0;
        byte []p;
        p = unit.src;
        System.out.println("Thread " + unit.id + " start\n");
        //System.out.println("PE %d\n", PE);
        //j 行数, i列数
        for (int j = unit.begin; j < unit.end; j++){
            for (int i = 0; i < dim; i++){
                a = 0;
                b = 0;
                c = 0;
                d = 0;
                n = 0;
                while ((c = a * a) + (d = b * b) < 4 && n++ < 880){
                    b = 2 * a * b + j * 8e-9 - .645411;
                    a = c - d + i * 8e-9 + .356888;
                }
                p[pos] = (byte)(255 * Math.pow((n - 80) / 800., 3.));
                p[pos + 1] = (byte)(255 * Math.pow((n - 80) / 800., .7));
                p[pos + 2] = (byte)(255 * Math.pow((n - 80) / 800., .5));
                p[pos] &= 255;
                p[pos + 1] &= 255;
                p[pos + 2] &= 255;
                pos += 3;
            }
        }
         System.out.println("Thread " + unit.id);
    }
    
    public static void main(String[] args) {
	// write your code here
        byte  []data = new byte[dim * dim * 3];
        String head =  new String("P6\n"+ dim + " " + dim+"\n255\n");
        Arg testUnit = new Arg();
        File file =new File("mathpic.ppm");
        testUnit.id = 0;
        testUnit.begin = 0;
        testUnit.end = dim;
        testUnit.src = data;
        long timeStart=System.currentTimeMillis();
        pixelWrite(testUnit);


        //if file doesnt exists, then create it
        if(!file.exists()){
            try {
                file.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        FileOutputStream fos = null;
        try {
             fos = new FileOutputStream(file);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        try {
            fos.write(head.getBytes());
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            fos.write(data);
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("Using time : "+(System.currentTimeMillis()-timeStart)+" ms ");
        System.out.println("Done");
    }
}

class Arg{
    int id;
    int begin;
    int end;
    byte []src;
}
