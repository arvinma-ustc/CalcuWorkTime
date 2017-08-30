function serialout = turbo_encode(input,codeRate,interleave_pattern)
%% 参数设置
g = [1 1 0 1;
     1 1 1 1];
f = [1 0 1 1]; 
m = 3;
len = length(input);
output = zeros(6, len + 2*m);

%% 编码器1
state = [0 0 0];
for k = 1 : len + 2*m
    % 计算rsc编码器输入
    if k <= len
        rsc_in = rem([input(k) state] * f(1, :).', 2);
        output(1, k) = input(k);
    else
        rsc_in =0; 
        output(1, k) = 0;
    end
    
    % 计算rsc编码器输出
    output(2, k) = rem([rsc_in state] * g(1, :).', 2);
    output(3, k) = rem([rsc_in state] * g(2, :).', 2);
    state = [rsc_in state(1 : end - 1)];
end

%% 交织
interleave_data = input(interleave_pattern);

%% 编码器2
state = [0 0 0];
for k = 1 : len + 2*m
    % 计算rsc编码器输入
    if k <= len
        rsc_in = rem([interleave_data(k) state] * f(1, :).', 2);
        output(4, k) = interleave_data(k);
    else
        rsc_in = 0;
        output(4, k) = 0;
    end
    
    % 计算rsc编码器输出
    output(5, k) = rem([rsc_in state] * g(1, :).', 2);
    output(6, k) = rem([rsc_in state] * g(2, :).', 2);
    state = [rsc_in state(1 : end - 1)];
end

%% 删余 
if codeRate == 1/2
   for i = 1:len + 2*m
       if i <= len
           if mod(i,2)==1
               serialout(2*i-1:2*i)=[output(1,i) output(2,i)];
           elseif mod(i,2)==0
               serialout(2*i-1:2*i)=[output(1,i) output(5,i)];
           end
       else         
           if i<=len+m
               serialout(2*i-1:2*i) = [output(1,i) output(2,i)];     %前三个尾部时钟
           else
               serialout(2*i-1:2*i) = [output(4,i) output(5,i)];     %后三个尾部时钟
           end
       end
   end
end

if codeRate == 3/4
   cnt = 1;
   for i = 1:len + 2*m
       if i <= len
           if mod(i,6)==1
               serialout(cnt:cnt+1)=[output(1,i) output(3,i)];
               cnt = cnt+2;
           elseif mod(i,6)==2 || mod(i,6)==3 || mod(i,6)==4 || mod(i,6)==5
               serialout(cnt)=output(1,i);
               cnt = cnt+1;            
           elseif mod(i,6)==0
               serialout(cnt:cnt+1)=[output(1,i) output(6,i)];
               cnt = cnt+2;
           end
       else         
           if i<=len+m
               serialout(cnt:cnt+1) = [output(1,i) output(3,i)];
               cnt = cnt+2;
           else
               serialout(cnt:cnt+1) = [output(4,i) output(6,i)];
               cnt = cnt+2;
           end
       end
   end
end

end
