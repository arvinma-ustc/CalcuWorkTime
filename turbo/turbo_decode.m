function output = turbo_decode(input,codeRate,interleave_pattern, source)

%% 还原turbo码的输入
m = 3;
if codeRate == 1/2   
    len = length(input)/2 - 2*m;
    lenWithTail = length(input)/2;
    sys_info = input(1:2:end);
    check_info = input(2:2:end);   
    % rsc1的输入
    rsc_in1 = zeros(2,lenWithTail);
    rsc_in1(1,1:end) = sys_info(1:end);
    rsc_in1(2,1:2:end) = check_info(1:2:end);
    % rsc2的输入
    rsc_in2 = zeros(2,lenWithTail);
    rsc_in2(1, 1:len) = sys_info(1, interleave_pattern);
    rsc_in2(1, end-5:end) = sys_info(1,end-5:end);
    rsc_in2(2,2:2:end) = check_info(2:2:end);
end

if codeRate == 3/4   
    len = (length(input)-2*2*m)*3/4;     %去掉尾部12个比特
    lenWithTail = len + 2*m;
    sys_info = zeros(1,lenWithTail);
    check_info = zeros(1,lenWithTail);
    
    cnt = 1;
    for n = 1: length(input)-2*2*m
        if mod(n,8)==2 || mod(n,8)==0
            check_info(cnt-1) = input(n);
        else
             sys_info(cnt) = input(n);           
             cnt = cnt+1;
        end
    end
    for n = length(input)-2*2*m+1 : length(input)
        if mod(n,2)==0
            check_info(cnt-1) = input(n);
        else          
            sys_info(cnt) = input(n);
            cnt = cnt+1;
        end
    end
    
    % rsc1的输入
    rsc_in1 = zeros(2,lenWithTail);
    rsc_in1(1,1:end) = sys_info(1:end);
    rsc_in1(2,1:2:end) = check_info(1:2:end);
    % rsc2的输入
    rsc_in2 = zeros(2,lenWithTail);
    rsc_in2(1, 1:len) = sys_info(1, interleave_pattern);
    rsc_in2(1, end-5:end) = sys_info(1,end-5:end);
    rsc_in2(2,2:2:end) = check_info(2:2:end);
end

%% turbo译码
iteras = 4;
apri = zeros(1, len + 2*m);
for it = 1 : iteras
    % 第一个rsc的译码
    [tmp,apri] = max_log_map(rsc_in1, apri);
    apri(1:end-6) = apri(interleave_pattern);
    apri(end - 5 : end) = 0;
    % 第二个rsc的译码
    [soft_value2, apri] = max_log_map(rsc_in2, apri);
    apri(interleave_pattern) = apri(1:end-6);
    apri(end - 5 : end) = 0;
    
    %% 译码结束，输出
    soft_value2(interleave_pattern) = soft_value2(1 : len);
    output = (sign(soft_value2(1 : len)) + 1) / 2;
    coded_ber = sum(abs(output - source))
end

