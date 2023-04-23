extern crate csv;

use std::io;
use std::error::Error;
use std::fs::File;
use std::io::BufWriter;

#[no_mangle]
pub extern "C" fn add(left: usize, right: usize) -> usize {
    left + right
}

// function we can call from arduino
pub extern "C" fn write_a_csv(data: [i64; 20], file_name: &str) -> Option<()> {
    let mut rdr = csv::Reader::from_reader(io::stdin());
    for result in rdr.records() {
        // The iterator yields Result<StringRecord, Error>, so we check the
        // error here..
        let record = result.ok()?;
        println!("{:?}", record);
    }
    return Some(());

    unimplemented!();
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn try_input() {
        write_a_csv([0; 20], "asdf");
        let one = 1;
        assert_eq!(one, 1);
    }

    // TODO: test for that function using csv
}
