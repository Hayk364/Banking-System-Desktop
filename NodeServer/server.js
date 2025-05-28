import express from "express";
import cors from "cors";
import { MongoClient } from "mongodb";
import crypto, { randomBytes } from "crypto"
import dotenv from 'dotenv'
import { send } from "process";
import { fileURLToPath } from "url";
import { error } from "console";
import events  from "events"

dotenv.config()

const GLOBALKEY = process.env.GLOBAL_KEY

const iv = crypto.randomBytes(16)

const emmiter = new events.EventEmitter()


function encrypt(text) {
    const cipher = crypto.createCipheriv('aes-256-cbc',GLOBALKEY,iv);
    let encrypted = cipher.update(text,'utf-8','hex')
    encrypted += cipher.final('hex')
    return{
        iv: iv.toString('hex'),
        encryptedData: encrypted
    }
}

function decrypt(encryptedData,ivHex){
    const decpiher = crypto.createDecipheriv('aes-256-cbc',GLOBALKEY,Buffer.from(ivHex,'hex'))
    let decrypted = decpiher.update(encryptedData,'hex','utf-8')
    decrypted+=decpiher.final('utf-8')
    return decrypted
}

function RandToken(){ return randomBytes(32).toString('hex') }

const PORT = 4000;
const uri = "mongodb://localhost:27017";
const dbName = "Bank";

const app = express();

app.use(cors());
app.use(express.json());

let db;

const client = new MongoClient(uri);
try {
    await client.connect();
    db = client.db(dbName); 
    console.log("Database connected successfully");
} catch (error) {
    console.error("Client connection error:", error);
    process.exit(1); 
}

app.get('/get-balance/:id',async (req,res) => {
  try {
    const id = parseInt(req.params.id)
    const user = await db.collection("wallet").findOne({ id: id });

    if (!user || !user.ballance) {
      return res.status(404).json({ error: "User not found or no balance" });
    }

    let balance = decrypt(user.ballance.encryptedData, user.ballance.iv);
    res.json({ balance: parseFloat(balance) });

  } catch (error) {
    console.log(error)
    res.status(500).json({ error: "Server error" });
  }
})


app.post("/register", async (req, res) => {
  try {
    const username = req.body.username;
    const password = encrypt(req.body.password)
    const lastElement = await db.collection("users").find().sort({ _id:-1 }).limit(1).toArray()
    const nextId = (lastElement[0]?.id ?? 0) + 1;
    const result = await db.collection("users").insertOne({name:username,password:{iv:password.iv,encryptedData:password.encryptedData},id:nextId})

    res.json({success: true,token:encrypt(username).encryptedData,id:nextId});
  } catch (err) {
    console.error("Error in /new-message:", err);
    res.json({ error: "Internal Server Error" });
  }
});

app.post("/login",async (req, res) => {
  try {
    const username = req.body.username;
    const password = req.body.password;
    const result = await db.collection("users").findOne({name:username})

    if (!result) {
      return res.json({ success: false, token: "", id: -1 });
    }

    const decryptPassword = decrypt(result.password.encryptedData,result.password.iv)
    console.log("User Password: ", password, "  : DB Password: ",decryptPassword)
    if(decryptPassword === password){
        res.json({success:true,token:encrypt(username).encryptedData,id:parseInt(result.id)})
    }else{
        res.json({success:false,token:"",id:-1})
    }

  } catch (err) {
    console.error("Error in /get-message: ",err);
    res.json({ error: "Internal Server Error" });
  }
});

app.post("/create-wallet",async (req,res) => {
    try {
        const walletData = req.body;
        const lastElement = await db.collection("wallet").find().sort({ _id: -1}).limit(1).toArray();
        const lastId = lastElement.length > 0 ? lastElement[0].id : 0;
        const balance = encrypt((0.0).toString())
        const result = await db.collection("wallet").insertOne({nameAndsurname:walletData.nameAndsurname,email:walletData.email,ballance:{iv:balance.iv,encryptedData:balance.encryptedData},id:lastId + 1,token:RandToken()})
        res.json({success:true})
    } catch (err) {
        console.log("Error create-wallet: ",err)
        res.json({success:false})
    }
})

app.post("/get-user-data", async (req, res) => {
  try {
      const username = req.body.username;
      const User = await db.collection("users").findOne({name:username})
      const result = await db.collection("wallet").findOne({ id: User.id });

      if (!result) {
          return res.status(404).json({ error: "User not found" });
      }
      let balance = 0.0;
      if (result.ballance && result.ballance.encryptedData && result.ballance.iv) {
          balance = parseFloat(decrypt(result.ballance.encryptedData, result.ballance.iv));
      }

      res.json({ token: result.token, balance });

  } catch (error) {
      console.error("Ошибка на сервере:", error);
      res.status(500).json({ error: "Internal server error" });
  }
});

app.post('/send',async (req,res) => {
  try {
    const token = req.body.token
    const count = req.body.count
    const id = req.body.id

    const sendUser = await db.collection("wallet").findOne({token:token})
    const userBalance = await db.collection("wallet").findOne({id:id})

    if(!sendUser || !userBalance){
      res.json({success:false,error:"Not Found User"})
    }

    let balance = parseFloat(decrypt(userBalance.ballance.encryptedData,userBalance.ballance.iv))

    if(balance - count - (count / 100 * 10) >= 0){
      balance = balance - count - (count / 100 * 10);
      balance = encrypt(balance.toString())
      await db.collection("wallet").updateOne(
        {id:id},
        {$set:{ballance:balance}}
      )
      balance = parseFloat(decrypt(sendUser.ballance.encryptedData,sendUser.ballance.iv))
      balance = balance + count
      balance = encrypt(balance.toString())
      await db.collection("wallet").updateOne(
        {token:token},
        {$set:{ballance:balance}}
      )
      balance = parseFloat(decrypt(sendUser.ballance.encryptedData,sendUser.ballance.iv))
      balance = balance + (count / 100 * 10)
      balance = encrypt(balance.toString())
      await db.collection("wallet").updateOne(
        {id:1},
        {$set:{ballance:balance}}
      )
      const addTransaction = await db.collection("history").insertOne({senderId:id,getterId:sendUser.id,count:count})
      res.json({success:true,error:""})
    }else{
      res.json({success:false,error:"Balance not value"})
    }

  } catch (error) {
    console.log(error)
  }
})


app.post("/get-name-id",async (req,res) => {
  try{
    const id = req.body.id;
    const result = await db.collection("users").findOne({id:id});
    if(!result) {
      res.json({name:"error"})
    }else{
      res.json({name:result.name})
    }
  }catch(error){
    console.log(error)
  }
})

app.post("/get-history",async (req,res) => {
  try{
    const id = req.body.id;

    const history = await db.collection("history").find({ $or:[ {senderId:id}, {getterId:id}]}).toArray();

    history.forEach(el => {
      console.log(el)
    })
    res.json(history) 
  }catch(error){
    console.log(error)
  }
})


app.get("/wallets",async (req,res) => {
  try {
    const result = await db.collection("wallet").find().toArray()
    for(const user of result){
      let balance = decrypt(user.ballance.encryptedData,user.ballance.iv)
      console.log("Name And Surname: ",user.nameAndsurname)
      console.log("Email: ", user.email)
      console.log("Balance: ", balance)
      console.log("Id: ",user.id)
      console.log("Token: ",user.token)
      console.log("-----------------------------")
    }
  } catch (error) {
    console.log("Error: ",error)
  }
})

app.get("/users",async (req,res) => {
  try {
    const result = await db.collection("users").find().toArray()
    for(const user of result){
      let password = decrypt(user.password.encryptedData,user.password.iv)
      console.log("Name",user.name)
      console.log("Id: ",user.id)
      console.log("Password: ",password)
      console.log("-----------------------------")
    }
  } catch (error) {
    console.log("Error: ",error)
  }
})

app.post('/get-name-surname',async (req,res) => {
  try {
    const id = req.body.id

    const name = await db.collection("wallet").findOne({id:id})
    res.json({name:name.nameAndsurname})
  } catch (error) {
    console.log(error)
  }
})

app.get("/add-user-balance/:userid/:count",async (req,res) => {
  try {
    const userId = req.params.userid
    const count = req.params.count
    const user = await db.collection("wallet").findOne({id:parseInt(userId)})
    let balance = encrypt((parseFloat(decrypt(user.ballance.encryptedData,user.ballance.iv)) + parseFloat(count)).toString())

    await db.collection("wallet").updateOne(
      {id:parseInt(userId)},
      {$set:{ballance:balance}}
    )
    console.log(true)
  } catch (error) {
    console.log(error)
  }
})

app.listen(PORT, () => {
  console.log(`Server Started on port ${PORT}`);
}).on('error', (err) => {
  console.error("Server error:", err);
});