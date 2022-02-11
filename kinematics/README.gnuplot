wget https://bitbucket.org/MattHawkinsUK/rpispy-misc/raw/master/gnuplot/gnuplot_generate_data.py
python gnuplot_generate_data.py


https://www.raspberrypi-spy.co.uk/2014/04/how-to-use-gnuplot-to-graph-data-on-the-raspberry-pi/

http://psy.swansea.ac.uk/staff/carter/gnuplot/gnuplot_3d.htm
wget http://psy.swansea.ac.uk/staff/carter/gnuplot/xyz.tsv

set key off

splot 'xyz.tsv' using 1:2:3 with points palette pointsize 3 pointtype 7

