function pattern = interleave(data_len)
if data_len==23552
    k1=8;
    k2=2944;
elseif data_len==128
    k1=2;
    k2=64;
elseif data_len==1920
    k1=4;
    k2=480;
elseif data_len==23056
    k1=8;
    k2=2882;
elseif data_len==136
    k1=2;
    k2=68;
elseif data_len==296
    k1=2;
    k2=148;
elseif data_len==32800
    k1=10;
    k2=3280;
end
p = [31 37 43 47 53 59 61 67];
pattern = zeros(1,data_len);
for s = 1:data_len
    m=mod(s-1,2);
    i=floor((s-1)/(2*k2));
    j=floor((s-1)/2)-i*k2;
    t=mod((19*i+1),k1/2);
    q=mod(t,8)+1;
    c=mod((p(q)*j+21*m),k2);
    pattern(s)=2*(t+c*k1/2+1)-m;
end

end