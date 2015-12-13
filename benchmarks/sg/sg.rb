
# TODO: make this a parameter
N= (ARGV[0].to_i != 0) ? ARGV[0].to_i : 75

# fix a and f
a=1
f=2

# initialize vector b, c, d and e
b = Array.new(N)
b.map!{ rand(N) }

c = Array.new(N)
c.map!{ rand(N) }

d = Array.new(N)
d.map!{ rand(N) }

e = Array.new(N)
e.map!{ rand(N) }


# create directory
Dir.mkdir("sg#{N}") unless Dir.exist?("sg#{N}")

# create up file
File.open("sg#{N}/up.facts",'w') do |f|
    (1..N).each do |i| 
        f.write("#{a}\t#{b[i]}\n")
    end
    (1..N).each do |i| 
        (1..N).each do |j|
            f.write("#{b[i]}\t#{c[j]}\n")
        end
    end
end

# create flat file
File.open("sg#{N}/flat.facts",'w') do |f|
    (1..N).each do |i| 
        (1..N).each do |j|
            f.write("#{c[i]}\t#{d[j]}\n")
        end
    end
end

# create down file
File.open("sg#{N}/down.facts",'w') do |f|
    (1..N).each do |i| 
        f.write("#{e[i]}\t#{f}\n")
    end
    (1..N).each do |i| 
        (1..N).each do |j|
            f.write("#{d[i]}\t#{e[j]}\n")
        end
    end
end
