plot;
subplot(1,4,1);
plt("plot \'inrad.prn\', \'rnet.prn\'");
subplot(1,4,2);
plt("plot \'rh.prn\'");
subplot(1,4,3);
plt("plot \'precip.prn\' with boxes");
subplot(1,4,4);
plt("plot \'wind.prn\',\'newt.prn\'");
endsubplot();
