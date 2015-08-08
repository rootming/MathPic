//#pragma OPENCL EXTENSION cl_intel_printf : enable
//#pragma OPENCL EXTENSION cl_khr_fp64 : enable
//#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
//#pragma optionNV(fastmath off)
//#pragma optionNV(fastprecision off)
//__kernel void vecAdd(__global char *source)
__kernel void vecAdd(__global char *source, const int line, const int pos)
{
        float a = 0, b = 0, c = 0, d = 0, n = 0;
		int gid = get_global_id(0);
        int i, j;
		i = gid / line + pos / line;
		j = gid % line ;


        while (c + d < 4. && n < 880.){
				c = a * a;
				d = b * b;
				b = 2. * a * b + j * 8e-9 -.645411;
                a = c - d + i * 8e-9 + .356888;
                n += 1;
        }
		source[gid * 3] = (char)(255. * native_powr((n - 80.) / 800., 3.)) & 255;
		source[gid * 3 + 1] = (char)(255. * native_powr((n - 80.) / 800., .7)) & 255;
		source[gid * 3 + 2] = (char)(255. * native_powr((n - 80.) / 800., .5)) & 255;
		//source[gid * 3] = 155;
		//source[gid * 3 + 1] = 55;
		//source[gid * 3 + 2] = 33;

}
