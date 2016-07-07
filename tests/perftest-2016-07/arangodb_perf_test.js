'use strict'
var fs = require('fs')

function load_data(directory, col){
    print("get files")
    var files = fs.list(directory)
    files.sort()

    var buffer;
    var uid = 0;
    files.forEach( function(file){
        print("reading: " + file)
        buffer=fs.readFileSync(directory + fs.pathSeparator + file)
        var lines = buffer.toString().split('\n')

        lines.forEach(function(line){
            col.save({ "_key" : uid.toString()
                     , "data" : line
                     });
            uid += 1;
        })
    })
    return col
}

function get_collection(name, drop){
    print("enter create collection")
    if (drop){
        print("drop")
        db._drop(name)
    }
    var col = db[name];
    if (col) { return col; }
    print("create new")
    return db._create(name)
}

function run_test(col){
    //ask for max key
    //random access key
    //replace randomenss later with a fixed list of keys
    //do timing

}

function main(){
    const drop_db = false
    var col = get_collection("obi-perf-test",drop_db)
    if(drop_db){
        load_data('/home/arango/testdata', col)
    }
    var time = run_test(col)
}

main()
