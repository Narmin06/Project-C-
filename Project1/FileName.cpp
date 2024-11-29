#include<iostream>
#include <windows.h>
#include<vector>
#include<string>
#include<fstream>
using namespace std;

string Encrypt(string input, int shift) {
    string result = "";
    for (int i = 0; i < input.length(); i++) {
        char ch = input[i];
        if (isupper(ch)) {
            result += char(int(ch + shift - 65) % 26 + 65);
        }
        else if (islower(ch)) {
            result += char(int(ch + shift - 97) % 26 + 97);
        }
        else if (isdigit(ch)) {
            result += char(int(ch + shift - 48) % 10 + 48);
        }
        else {
            result += ch;
        }
    }
    return result;
}

string Decrypt(string input, int shift) {
    string result = "";
    for (int i = 0; i < input.length(); i++) {
        char ch = input[i];
        if (isupper(ch)) {
            result += char(int(ch - shift - 65 + 26) % 26 + 65);
        }
        else if (islower(ch)) {
            result += char(int(ch - shift - 97 + 26) % 26 + 97);
        }
        else if (isdigit(ch)) {
            result += char(int(ch - shift - 48 + 10) % 10 + 48);
        }
        else {
            result += ch;
        }
    }
    return result;
}

class Admin {
    string username;
    string password;
public:
    Admin() {
        string username = "";
        string password = "";
    }

    Admin(string username, string password) {
        SetUsername(username);
        SetPassword(password);
    }

    string GetUsername() const { return this->username; }
    string GetPassword() const { return this->password; }

    void SetUsername(string username) {
        if (username.length() > 4) {
            this->username = Encrypt(username, 3);
        }
        else {
            cout << "Short Username" << endl;
        }
    }

    void SetPassword(string password) {
        if (password.length() > 4) {
            this->password = Encrypt(password, 3);
        }
        else {
            cout << "Short Password" << endl;
        }
    }

    void SaveToFile() const{
        fstream file("admin.txt", ios::out);
        if (!file) {
            cout << "File doesn't open for write!!!" << endl;
            return;
        }
        file << username << endl;
        file << password << endl;
        file.close();
    }

    static shared_ptr<Admin> LoadFromFile() {
        fstream file("admin.txt", ios::in);
        if (!file) {
            cout << "Fayl tapılmadı!" << endl;
            return nullptr;
        }
        string username, password;
        getline(file, username);
        getline(file, password);
        file.close();
        shared_ptr<Admin> admin = make_shared<Admin>(Decrypt(username, 3), Decrypt(password, 3));
        return admin;
    }
};

class Question {
protected:
    string questionText;
    string correctAnswer;
    vector<string> choices;
public:
    Question(string question, string answer) {
        this->questionText = question;
        this->correctAnswer = answer;
    }
    virtual void ShowQuestion() = 0;
    virtual bool CheckAnswer(string userChoice) = 0;
    virtual void SaveToFile(ofstream& outFile) const = 0;
};

class OpenQuestion : public Question {
public:
    OpenQuestion(string question, string answer) : Question(question, answer) {}
    void ShowQuestion() override {
        cout << questionText << endl;
    }
    bool CheckAnswer(string userAnswer) override {
        return userAnswer == correctAnswer;
    }
    void SaveToFile(ofstream& outFile) const override {
        outFile << "OpenQuestion\n";
        outFile << questionText << endl;
        outFile << correctAnswer << endl;
    }
};

class TwoChoiceQuestion : public Question {
    void shuffleChoices() {
        srand(time(NULL));
        for (size_t i = 0; i < choices.size() - 1; i++) {
            int randomIndex = rand() % (i + 1);
            swap(choices[i], choices[randomIndex]);
        }
    }
public:
    TwoChoiceQuestion(string question, string correct, string wrong) : Question(question, correct) {
        this->choices.push_back(correct);
        this->choices.push_back(wrong);
        shuffleChoices();
    }

    void ShowQuestion() override {
        cout << questionText << endl;
        for (int i = 0; i < choices.size(); i++) {
            cout << i + 1 << ". " << choices[i] << endl;
        }
    }

    bool CheckAnswer(string userChoice) override {
        int answerChoice = stoi(userChoice);
        return (choices[answerChoice - 1] == correctAnswer);
    }

    void SaveToFile(ofstream& outFile) const override {
        outFile << "TwoChoiceQuestion\n";
        outFile << questionText << endl;
        outFile << correctAnswer << endl;
        outFile << choices[0] << endl;
        outFile << choices[1] << endl;
    }
};

class FourChoiceQuestion : public Question {
    void shuffleChoices() {
        srand(time(NULL));
        for (size_t i = 0; i < choices.size() - 1; i++) {
            int randomIndex = rand() % (i + 1);
            swap(choices[i], choices[randomIndex]);
        }
    }
public:
    FourChoiceQuestion(string question, string correct, vector<string> wrongChoices) : Question(question, correct) {
        this->choices.push_back(correct);
        this->choices.insert(choices.end(), wrongChoices.begin(), wrongChoices.end());
        shuffleChoices();
    }

    void ShowQuestion() override {
        cout << questionText << endl;
        for (int i = 0; i < choices.size(); i++) {
            cout << i + 1 << ". " << choices[i] << endl;
        }
    }

    bool CheckAnswer(string userChoice) override {
        int answerChoice = stoi(userChoice);
        return (choices[answerChoice - 1] == correctAnswer);
    }

    void SaveToFile(ofstream& outFile) const override {
        outFile << "FourChoiceQuestion\n";
        outFile << questionText << endl;
        outFile << correctAnswer << endl;
        for (const string& choice : choices) {
            outFile << choice << endl;
        }
    }
};

class SubCategory {
    string name;
    vector<shared_ptr<Question>> questions;
public:
    SubCategory(string categoryName) : name(categoryName) {}

    string GetSubcategoryName() const { return name; }

    vector<shared_ptr<Question>> GetSubcategoryQuestions() const { return questions; }

    void AddQuestion(shared_ptr<Question> question) {
        questions.push_back(question);
    }

    void ShowQuestions() {
        cout << "Questions in SubCategory: " << name << endl;
        for (size_t i = 0; i < questions.size(); i++)
        {
            cout << "\nTest" << i + 1 << " - ";
            questions[i]->ShowQuestion();
        }
        cout << endl;
    }

    void DeleteQuestion(int index) {
        if (index >= 0 && index < questions.size()) {
            questions.erase(questions.begin() + index);
            cout << "Question successfully deleted" << endl;
        }
        else {
            cout << "You entered the wrong number!" << endl;
        }
    }
    void SaveToFile(ofstream& outFile) const {
        outFile << name << endl;
        outFile << questions.size() << endl;
        for (const auto& question : questions) {
            question->SaveToFile(outFile);
        }
    }
};

class Category {
    string name;
    vector<shared_ptr<SubCategory>> subCategories;
public:
    Category(string categoryName) : name(categoryName) {}

    string GetCategoryName() const { return name; }

    vector<shared_ptr<SubCategory>> GetSubCategories() const { return subCategories; }

    void AddSubCategory(shared_ptr<SubCategory> subCategory) {
        subCategories.push_back(subCategory);
    }

    void ShowSubCategories() const {
        cout << "SubCategories: " << endl;
        for (size_t i = 0; i < subCategories.size(); i++) {
            cout << i + 1 << ". " << subCategories[i]->GetSubcategoryName() << endl;
        }
    }

    void SaveToFile(ofstream& outFile) const {
        outFile << name << endl;
        outFile << subCategories.size() << endl;
        for (const auto& subCategory : subCategories) {
            subCategory->SaveToFile(outFile);
        }
    }
};

class User {
    string firstName;
    string lastName;
    string fatherName;
    string address;
    string phone;
    string username;
    string password;
    int user_id;
    vector<string> testResults;
    int correctAnswers = 0;
public:
    int lastTestQuestionIndex = -1;
    static int static_id;
    User(string firstName, string lastName, string fatherName, string address, string phone, string username, string password) {
        SetFirstName(firstName);
        SetLastName(lastName);
        SetFatherName(fatherName);
        SetAddress(address);
        SetPhone(phone);
        SetUsername(username);
        SetPassword(password);
        static_id++;
        this->user_id = static_id;
    }

    string GetFirstName() const { return this->firstName; }
    string GetLastName() const { return this->lastName; }
    string GetFatherName() const { return this->fatherName; }
    string GetAddress() const { return this->address; }
    string GetPhone() const { return this->phone; }
    string GetUsername() const { return this->username; }
    string GetPassword() const { return this->password; }
    int GetID() const { return user_id; }

    void SetFirstName(string firstname) {
        if (firstname.length() > 2) {
            this->firstName = firstname;
        }
        else {
            cout << "Short Firstname!!!" << endl;
        }
    }
    void SetLastName(string lastname) {
        if (lastname.length() > 5) {
            this->lastName = lastname;
        }
        else {
            cout << "Short Lastname!!!" << endl;
        }
    }
    void SetFatherName(string fathername) {
        if (fathername.length() > 2) {
            this->fatherName = fathername;
        }
        else {
            cout << "Short Fathername!!!" << endl;
        }
    }
    void SetAddress(string address) {
        if (address.length() > 3) {
            this->address = address;
        }
        else {
            cout << "Wrong Address!!!" << endl;
        }
    }
    void SetPhone(string phone) {
        if (phone.length() > 10) {
            this->phone = phone;
        }
        else {
            cout << "Wrong Phone!!!" << endl;
        }
    }
    void SetUsername(string username) {
        if (username.length() > 4) {
            this->username = Encrypt(username, 3);
        }
        else {
            cout << "Short Username" << endl;
        }
    }
    void SetPassword(string password) {
        if (password.length() > 4) {
            this->password = Encrypt(password, 3);
        }
        else {
            cout << "Short Password" << endl;
        }
    }

    void ShowUserInfo() const {
        cout << "----------" << GetID() << "-----------" << endl;
        cout << "Firstname: " << GetFirstName() << endl;
        cout << "Lastname: " << GetLastName() << endl;
        cout << "Fathername: " << GetFatherName() << endl;
        cout << "Address: " << GetAddress() << endl;
        cout << "Phone: " << GetPhone() << endl;
        cout << "Username: " << GetUsername() << endl;
        cout << "Password: " << GetPassword() << endl;
    }

    void ViewTestsResults() {
        if (testResults.empty()) {
            cout << "No test result" << endl;
        }
        else {
            for (size_t i = 0; i < testResults.size(); i++)
            {
                cout << testResults[i] << endl;
            }
        }
    }

    void AddResult(string result) {
        testResults.push_back(result);
    }
    
    vector<string> GetTestResults() const {
        return testResults;
    }

    void IncrementCorrectAnswers() {
        correctAnswers++;
    }

    int GetCorrectAnswers() const {
        return correctAnswers;
    }

    void TakeTest(shared_ptr<Category> selectedCategory);
    void ContinueTest(shared_ptr<Category> selectedCategory);
};

int User::static_id = 0;

void User::TakeTest(shared_ptr<Category> selectedCategory) {
    selectedCategory->ShowSubCategories();
    int choiceSubCategory;
    cout << "Select subcategory: ";
    cin >> choiceSubCategory;
    cin.ignore(); 

    shared_ptr<SubCategory> selectedSubCategory = selectedCategory->GetSubCategories()[choiceSubCategory - 1];

    int lastQuestionIndex = lastTestQuestionIndex;
    int totalQuestions = selectedSubCategory->GetSubcategoryQuestions().size();
    int userCorrectAnswers = 0;

    cout << "----------START TESTS----------" << endl;
    if (lastQuestionIndex == -1) {
        lastQuestionIndex = 0; 
    }

    for (size_t i = lastQuestionIndex; i < totalQuestions; i++) {  
        string testStop{};
        cout << "Testi dayandırmaq istəyirsiniz? (Yes / No)" << endl;
        cin >> testStop;
        cin.ignore(); 

        if (testStop == "No") {
            selectedSubCategory->GetSubcategoryQuestions()[i]->ShowQuestion();
            string userAnswer;
            cout << "Cavabınızı daxil edin: ";
            getline(cin, userAnswer);
            if (selectedSubCategory->GetSubcategoryQuestions()[i]->CheckAnswer(userAnswer)) {
                cout << "Doğru cavab!" << endl;
                userCorrectAnswers++;
                IncrementCorrectAnswers();
            }
            else {
                cout << "Səhv cavab!" << endl;
            }
        }
        else if (testStop == "Yes") {
            lastTestQuestionIndex = i;  
            //FAYLA YAZILMALIDIR!!!!!!!!!!!!!!!!!!!
            break;
        }
        else {
            cout << "Səhv cavab!" << endl;
        }
    }

    double percentage = static_cast<double>(userCorrectAnswers) / totalQuestions * 100;
    int grade = 0;

    if (percentage == 100) {
        grade = 12;
    }
    else if (percentage >= 90) {
        grade = 11;
    }
    else if (percentage >= 80) {
        grade = 10;
    }
    else if (percentage >= 70) {
        grade = 9;
    }
    else if (percentage >= 60) {
        grade = 8;
    }
    else if (percentage >= 50) {
        grade = 7;
    }
    else if (percentage >= 40) {
        grade = 6;
    }
    else if (percentage >= 30) {
        grade = 5;
    }
    else if (percentage >= 20) {
        grade = 4;
    }
    else if (percentage >= 10) {
        grade = 3;
    }
    else {
        grade = 2;
    }

    cout << "\nTest tamamlandı!" << endl;
    cout << "Düzgün cavablar: " << userCorrectAnswers << "/" << totalQuestions << endl;
    cout << "Cavablarınızın faizi: " << percentage << "%" << endl;
    cout << "Qiymət: " << grade << " / 12" << endl;

    string result = "Kategoriya: " + selectedCategory->GetCategoryName()
        + " - Subkateqoriya: " + selectedSubCategory->GetSubcategoryName() + " - Test nəticəsi: " + to_string(grade);
    AddResult(result);
}

void User :: ContinueTest(shared_ptr<Category> selectedCategory) {
    shared_ptr<SubCategory> selectedSubCategory = selectedCategory->GetSubCategories()[0]; 
    int totalQuestions = selectedSubCategory->GetSubcategoryQuestions().size();
    int startIndex = lastTestQuestionIndex + 1;
    for (int i = startIndex; i < totalQuestions; i++) {
        selectedSubCategory->GetSubcategoryQuestions()[i]->ShowQuestion();
        string userAnswer;
        cout << "Enter your answer: ";
        getline(cin, userAnswer);
        if (selectedSubCategory->GetSubcategoryQuestions()[i]->CheckAnswer(userAnswer)) {
            cout << "Doğru cavab!" << endl;
            IncrementCorrectAnswers();
        }
        else {
            cout << "Wrong answer!" << endl;
        }
        lastTestQuestionIndex = i;  // Testin hansı sualdan davam etdiyini saxlayırıq
    }
}

class Database {
    vector<shared_ptr<User>> users;
    shared_ptr<Admin> admin;
    vector<shared_ptr<Category>> categories;

public:
    void AddAdmin(shared_ptr<Admin> newAdmin) {
        if (admin != nullptr) {
            cout << "Admin already exists. Only one admin can be created..." << endl;
            return;
        }
        admin = newAdmin; 
        admin->SaveToFile(); 
    }

    bool AdminLogin(const string& username, const string& password) {
        fstream file("admin.txt", ios::in);
        if (!file) {
            cout << "No admin available!!!" << endl;
            return false;
        }

        string fileUsername, filePassword;
        while (getline(file, fileUsername) && getline(file, filePassword)) {
            string decryptedUsername = Decrypt(fileUsername, 3);
            string decryptedPassword = Decrypt(filePassword, 3);

            if (decryptedUsername == username && decryptedPassword == password) {
                cout << "Admin login successful" << endl;
                return true;
            }
        }
        cout << "Incorrect admin password or username!" << endl;
        return false;
    }

    //////////////////////////////////////// USER /////////////////////////////////////

    void AddUser(shared_ptr<User> newUser) {
        for (auto existingUser : users) {
            if (existingUser->GetUsername() == newUser->GetUsername()) {
                cout << "User already exists!" << endl;
                return;
            }
        }
        users.push_back(newUser);
        SaveUsersToFile();
    }

    void DeleteUser(int index) {
        if (index >= 0 && index < users.size()) {
            users.erase(users.begin() + index);
            SaveUsersToFile(); 
        }
        else {
            cout << "Invalid Index!" << endl;
        }
    }

    bool UserLogin(const string& username, const string& password) {
        fstream file("users.txt", ios::in);
        if (!file) {
            cout << "No user available!!!" << endl;
            return false;
        }

        string fileUsername, filePassword;
        while (getline(file, fileUsername) && getline(file, filePassword)) {
            string decryptedUsername = Decrypt(fileUsername, 3);
            string decryptedPassword = Decrypt(filePassword, 3);

            if (decryptedUsername == username && decryptedPassword == password) {
                cout << "User login successful" << endl;
                return true;
            }
        }
        cout << "Incorrect user password or username!" << endl;
        return false;
    }

    void ShowUsers() const {
        cout << "------------- Users List -------------" << endl;
        for (size_t i = 0; i < users.size(); i++)
        {
            cout << "-------------" << i + 1 << "-------------" << endl;
            users[i]->ShowUserInfo();
        }
    }

    void SaveUsersToFile() {
        fstream file("users.txt", ios::out | ios::trunc); 
        if (!file) {
            cout << "The file could not be opened!" << endl;
            return;
        }
        for (auto& user : users) {
            file << user->GetUsername() << endl;
            file << user->GetPassword() << endl;
            file << user->GetFirstName() << endl;
            file << user->GetLastName() << endl;
            file << user->GetFatherName() << endl;
            file << user->GetAddress() << endl;
            file << user->GetPhone() << endl;
            file << user->GetID() << endl;
            file << user->lastTestQuestionIndex << endl;

            for (const string& result : user->GetTestResults()) {
                file << result << endl;
            }
            file << "---END_USER---" << endl;
        }
        file.close();
    }

    bool ReadUsersFromFile(const string& filename) {
        fstream file(filename, ios::in);
        if (!file) {
            cout << "File not found!" << endl;
            return false;
        }
        string fileUsername, filePassword;
        string fileFirstName, fileLastName, fileFatherName, fileAddress, filePhone;
        int fileUserId, lastQuestionIndex;
        vector<string> fileTestResults;

        while (getline(file, fileUsername) && getline(file, filePassword)) {
            string decryptedUsername = Decrypt(fileUsername, 3);
            string decryptedPassword = Decrypt(filePassword, 3);

            getline(file, fileFirstName);
            getline(file, fileLastName);
            getline(file, fileFatherName);
            getline(file, fileAddress);
            getline(file, filePhone);
            file >> fileUserId;
            file >> lastQuestionIndex;
            file.ignore(); 
            fileTestResults.clear();
            string testResult;
            while (getline(file, testResult) && testResult != "---END_USER---") {
                fileTestResults.push_back(testResult);
            }
            shared_ptr<User> user = make_shared<User>(fileFirstName, fileLastName, fileFatherName, fileAddress, filePhone, decryptedUsername, decryptedPassword);
            user->lastTestQuestionIndex = lastQuestionIndex;
            for (const auto& result : fileTestResults) {
                user->AddResult(result);
            }
            users.push_back(user);
        }
        file.close();
        return true;
    }

    int UsersCount() const { return users.size(); }

    shared_ptr<User> GetIndexUser(int index) {
        if (index >= 0 && index < users.size()) {
            return users[index];
        }
        return nullptr;
    }

    vector<shared_ptr<User>>& GetUsers() {
        return users;
    }

    //////////////////////////////////////// Category /////////////////////////////////////

    void AddCategory(shared_ptr<Category> category) {
        categories.push_back(category);
    }

    void DeleteCategory(int index) {
        if (index >= 0 && index < categories.size()) {
            shared_ptr<Category> categoryToDelete = categories[index];

            // Subkateqoriyaları silirik
            categoryToDelete->GetSubCategories().clear();  // Bütün subkateqoriyalar silinir

            // Kateqoriyanı silirik
            categories.erase(categories.begin() + index);
            cout << "Category and its subcategories successfully deleted!" << endl;
        }
        else {
            cout << "Invalid Index!" << endl;
        }
    }

    void ShowCategories() const {
        cout << "------------- Categories List -------------" << endl;
        for (size_t i = 0; i < categories.size(); i++)
        {
            cout << i + 1 << "." << categories[i]->GetCategoryName() << endl;

            auto subCategories = categories[i]->GetSubCategories();
            if (subCategories.empty()) {
                cout << "   No subcategories available!" << endl;
            }
            else {
                cout << "   Subcategories:" << endl;
                for (size_t j = 0; j < subCategories.size(); j++) {
                    cout << "     " << j + 1 << ". " << subCategories[j]->GetSubcategoryName() << endl;
                }
            }
        }
    }

    shared_ptr<Category> GetIndexCategory(int index) {
        if (index >= 0 && index < categories.size()) {
            return categories[index];
        }
        return nullptr;
    }

    int CategoriesCount() const { return categories.size(); }

    void SaveToFile(const string& filename) const {
        ofstream outFile(filename, ios::trunc);
        if (!outFile) {
            cout << "Fayl açılmadı!" << endl;
            return;
        }
        outFile << categories.size() << endl;
        for (const auto& category : categories) {
            outFile << category->GetCategoryName() << endl;
            outFile << category->GetSubCategories().size() << endl;

            for (const auto& subCategory : category->GetSubCategories()) {
                outFile << subCategory->GetSubcategoryName() << endl;
                outFile << subCategory->GetSubcategoryQuestions().size() << endl;

                for (const auto& question : subCategory->GetSubcategoryQuestions()) {
                    question->SaveToFile(outFile);
                }
            }
        }
        outFile.close();
    }

    void LoadFromFile(const string& filename) {
        ifstream inFile(filename);
        if (!inFile) {
            cout << "The file could not be opened!" << endl;
            return;
        }
        int categoryCount;
        inFile >> categoryCount;
        inFile.ignore();
        categories.clear();

        for (size_t i = 0; i < categoryCount; i++) {
            string categoryName;
            getline(inFile, categoryName);

            shared_ptr<Category> newCategory(new Category(categoryName));
            int subCategoryCount;
            inFile >> subCategoryCount;
            inFile.ignore();

            for (size_t j = 0; j < subCategoryCount; j++) {
                string subCategoryName;
                getline(inFile, subCategoryName);
                shared_ptr<SubCategory> newSubCategory(new SubCategory(subCategoryName));

                int questionCount;
                inFile >> questionCount;
                inFile.ignore();

                for (size_t k = 0; k < questionCount; k++) {
                    string questionType;
                    getline(inFile, questionType);
                    string questionText, correctAnswer;
                    getline(inFile, questionText);
                    getline(inFile, correctAnswer);

                    if (questionType == "OpenQuestion") {
                        shared_ptr<Question> question(new OpenQuestion(questionText, correctAnswer));
                        newSubCategory->AddQuestion(question);
                    }
                    else if (questionType == "TwoChoiceQuestion") {
                        string wrongChoice1, wrongChoice2;
                        getline(inFile, wrongChoice1);
                        getline(inFile, wrongChoice2);
                        shared_ptr<Question> question(new TwoChoiceQuestion(questionText, correctAnswer, wrongChoice1));
                        newSubCategory->AddQuestion(question);
                    }
                    else if (questionType == "FourChoiceQuestion") {
                        string wrongChoice1, wrongChoice2, wrongChoice3;
                        getline(inFile, wrongChoice1);
                        getline(inFile, wrongChoice2);
                        getline(inFile, wrongChoice3);
                        vector<string> wrongChoices = { wrongChoice1, wrongChoice2, wrongChoice3 };
                        shared_ptr<Question> question(new FourChoiceQuestion(questionText, correctAnswer, wrongChoices));
                        newSubCategory->AddQuestion(question);
                    }
                }
                newCategory->AddSubCategory(newSubCategory);
            }
            categories.push_back(newCategory);
        }
        inFile.close();
    }

    //////////////////////////////////////// ShowResultMethods /////////////////////////////////////

    void ShowAllTestResults() {
        int totalTests = 0;
        int totalCorrectAnswers = 0;
        for (auto user : users) {
            totalTests += user->GetTestResults().size();
            totalCorrectAnswers += user->GetCorrectAnswers();
        }
        double result = 0;
        if (totalTests > 0) {
            result = static_cast<double>(totalCorrectAnswers) / totalTests * 100;
        }
        else {
            result = 0;
        }
        cout << "Ümumi keçirilən testlər: " << totalTests << endl;
        cout << "Düzgün cavabların sayı: " << totalCorrectAnswers << endl;
        cout << "Test uğurluluq faizi: " << result << "%" << endl;
    }

    void ShowCategoryTestResults(const int& categoryID) {
        int totalTests = 0;
        int totalCorrectAnswers = 0;
        if (categoryID < 0 || categoryID >= categories.size()) {
            cout << "Category not found!" << endl;
            return;
        }
        shared_ptr<Category> selectedCategory = categories[categoryID];

        for (const auto& subCategory : selectedCategory->GetSubCategories()) {
            for (const auto& user : users) {
                for (const auto& result : user->GetTestResults()) {
                    if (result.find(selectedCategory->GetCategoryName()) != string::npos) {
                        totalTests++;
                        if (result.find("Doğru cavab!") != string::npos) {
                            totalCorrectAnswers++;
                        }
                    }
                }
            }
        }
        double result = 0;
        if (totalTests > 0) {
            result = static_cast<double>(totalCorrectAnswers) / totalTests * 100;
        }
        else {
            result = 0;
        }
        cout << selectedCategory->GetCategoryName() << " üzrə ümumi keçirilən testlər: " << totalTests << endl;
        cout << selectedCategory->GetCategoryName() << " üzrə düzgün cavabların sayı: " << totalCorrectAnswers << endl;
        cout << selectedCategory->GetCategoryName() << " üzrə test uğurluluq faizi: " << result << "%" << endl;
    }

    void ShowSubcategoryResults(const string& subCategoryName) {
        int totalTests = 0;
        int totalCorrectAnswers = 0;
        shared_ptr<SubCategory> selectedSubCategory = nullptr;
        for (const auto& category : categories) {
            for (const auto& subCategory : category->GetSubCategories()) {
                if (subCategory->GetSubcategoryName() == subCategoryName) {
                    selectedSubCategory = subCategory;
                    break;  
                }
            }
            if (selectedSubCategory != nullptr) break;  
        }

        if (selectedSubCategory == nullptr) {
            cout << "Subcategory not found!!!" << endl;
            return;
        }

        for (const auto& user : users) {
            for (const auto& result : user->GetTestResults()) {
                if (result.find(selectedSubCategory->GetSubcategoryName()) != string::npos) {
                    totalTests++;
                    if (result.find("Doğru cavab!") != string::npos) {
                        totalCorrectAnswers++;
                    }
                }
            }
        }

        double result = 0;
        if (totalTests > 0) {
            result = static_cast<double>(totalCorrectAnswers) / totalTests * 100;
        }
        cout << selectedSubCategory->GetSubcategoryName() << " üzrə ümumi keçirilən testlər: " << totalTests << endl;
        cout << selectedSubCategory->GetSubcategoryName() << " üzrə düzgün cavabların sayı: " << totalCorrectAnswers << endl;
        cout << selectedSubCategory->GetSubcategoryName() << " üzrə test uğurluluq faizi: " << result << "%" << endl;
    }

    void ShowUserTestResults(const int& userId) {
        shared_ptr<User> selectedUser = nullptr;
        for (const auto& user : users) {
            if (user->GetID() == userId) {
                selectedUser = user;
                break;
            }
        }

        if (selectedUser == nullptr) {
            cout << "User not found!!!" << endl;
            return;
        }

        cout << "----------" << selectedUser->GetFirstName() << " " << selectedUser->GetLastName() << "-----------" << endl;
        selectedUser->ViewTestsResults();
    }

};

class InvalidChoiceException {
    string message;
public:
    InvalidChoiceException(string message) :message(message) {}

    string ShowMessage() const {
        return message;
    }
};

int My_Str_To_Num(string& str) {
    int sum = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] >= '0' && str[i] <= '9') {
            sum = sum * 10 + int(str[i] - 48);
        }
        else {
            return -1;
            break;
        }
    }
    return sum;
}

int WrongChoice() {
    string input;
    while (true) {
        cout << "Enter choice: ";
        getline(cin, input);
        try {
            int choice = My_Str_To_Num(input);
            if (choice == -1) {
                throw InvalidChoiceException("Invalid input! Please enter a valid number.");
            }
            return choice;
        }
        catch (const InvalidChoiceException& ex) {
            cout << ex.ShowMessage() << endl;
            Sleep(1500); 
            system("cls");
            cout << "1.Login" << endl;
            cout << "2.Sign up" << endl;
            cout << "3.Exit" << endl;
        }
    }
}

void main() {
    Database db;
    db.LoadFromFile("data.txt");
    db.ReadUsersFromFile("users.txt");
    while (true) {
        system("cls");
        cout << "1.Login" << endl;
        cout << "2.Sign up" << endl;
        cout << "3.Exit" << endl;

        string  choicee{};
        cout << "Enter choice: ";
        cin >> choicee;
        int choice = My_Str_To_Num(choicee);

        if (choice == 1) { // LOGIN
            system("cls");
            cout << "1.Admin" << endl;
            cout << "2.User" << endl;

            string choiceLoginStr{};
            cout << "Enter choice: ";
            cin >> choiceLoginStr;
            int choiceLogin = My_Str_To_Num(choiceLoginStr);
            cin.ignore();

            if (choiceLogin == 1) { // ADMIN LOGIN
                system("cls");
                string admin_username_login{}, admin_password_login{};

                cout << "Enter Admin username: ";
                cin >> admin_username_login;
                cin.ignore();

                cout << "Enter Admin password: ";
                cin >> admin_password_login;
                cin.ignore();
                system("cls");
                if (db.AdminLogin(admin_username_login, admin_password_login)) {
                    while (true) {
                        system("cls");
                        cout << "--------------- ADMIN ---------------\n\n";
                        cout << "1 - A look at the statistics\n";
                        cout << "2 - Changes related to users\n";
                        cout << "3 - Changes related to categories\n";
                        cout << "4 - Changes related to tests\n\n";
                        cout << "5 - Back" << endl;
                        string choice3Str{};
                        cout << "Enter choice: ";
                        cin >> choice3Str;
                        int choice3 = My_Str_To_Num(choice3Str);
                        cin.ignore();
                        system("cls");

                        if (choice3 == 1) { // Admin statistikalarla əlaqəli əməliyyatlar
                            while (true) {
                                system("cls");
                                cout << "1 - Ümumi test nəticələri" << endl;
                                cout << "2 - Kateqoriyalar üzrə test nəticələri" << endl;
                                cout << "3 - Konkret test üzrə nəticələr" << endl;
                                cout << "4 - Fərdi istifadəçi üzrə nəticələr" << endl;
                                cout << "5 - Bakc" << endl;
                                string statChoice;
                                cout << "Enter choice: ";
                                cin >> statChoice;
                                int statChoiceInt = My_Str_To_Num(statChoice);
                                if (statChoiceInt == 1) {
                                    db.ShowAllTestResults();
                                }
                                else if (statChoiceInt == 2) {
                                    db.ShowCategories();
                                    int categoryId{};
                                    cout << "Enter category ID: ";
                                    cin >> categoryId;
                                    db.ShowCategoryTestResults(categoryId);
                                }
                                else if (statChoiceInt == 3) {
                                    db.ShowCategories();
                                    string subCategoryName{};
                                    cout << "Enter subcategory name: ";
                                    cin >> subCategoryName;
                                    db.ShowSubcategoryResults(subCategoryName);
                                }
                                else if (statChoiceInt == 4) {
                                    db.ShowUsers();
                                    int userId;
                                    cout << "İstifadəçi ID-ni daxil edin: ";
                                    cin >> userId;
                                    db.ShowUserTestResults(userId);
                                }
                                else if (statChoiceInt == 5) {
                                    // Heç bir əməliyyat yoxdur, sadəcə geri qayıtmaq üçün
                                }
                                else {
                                    cout << "Yanlış seçim!" << endl;
                                }
                            }
                        }
                        else if (choice3 == 2) { // Admin userlərlə bağlı əməliyyatlar
                            while (true) {
                                system("cls");
                                cout << "1 - Show all Users" << endl;
                                cout << "2 - Add User" << endl;
                                cout << "3 - Delete User" << endl;
                                cout << "4 - User modification" << endl;
                                cout << "5 - Back" << endl;
                                int choiceUser{};
                                cout << "Enter choice: ";
                                cin >> choiceUser;
                                if (choiceUser == 1) {
                                    db.ShowUsers();
                                    Sleep(2000);
                                    system("cls");
                                }
                                else if (choiceUser == 2) {
                                    string user_firstname, user_lastname, user_fathername, user_address, user_phone, user_username, user_password;
                                    cout << "Enter user firstname: ";
                                    cin >> user_firstname;
                                    cin.ignore();
                                    cout << "Enter user lastname: ";
                                    getline(cin, user_lastname);
                                    cout << "Enter user fathername: ";
                                    getline(cin, user_fathername);

                                    cout << "Enter user address: ";
                                    getline(cin, user_address);

                                    cout << "Enter user phone: ";
                                    getline(cin, user_phone);

                                    cout << "Enter user username: ";
                                    getline(cin, user_username);

                                    cout << "Enter user password: ";
                                    getline(cin, user_password);

                                    shared_ptr <User> user(new User(user_firstname, user_lastname, user_fathername, user_address, user_phone, user_username, user_password));
                                    db.AddUser(user);
                                    cout << "User successfully added" << endl;
                                    Sleep(1500);
                                    system("cls");
                                }
                                else if (choiceUser == 3) {
                                    db.ShowUsers();
                                    int choiceDeleteUser{};
                                    cout << endl << "Select the user you want to delete: ";
                                    cin >> choiceDeleteUser;
                                    if (choiceDeleteUser > 0 && choiceDeleteUser <= db.UsersCount()) {
                                        db.DeleteUser(choiceDeleteUser - 1);
                                        cout << "User successfully deleted" << endl;
                                        db.ShowUsers();
                                        Sleep(1500);
                                        system("cls");
                                    }
                                    else {
                                        cout << "Invalid choice!" << endl;
                                        Sleep(1500);
                                        system("cls");
                                    }
                                }
                                else if (choiceUser == 4) {
                                    db.ShowUsers();
                                    int choiceUser{};
                                    cout << endl << "Select the user you want to change: ";
                                    cin >> choiceUser;
                                    Sleep(1500);
                                    system("cls");
                                    if (choiceUser > 0 && choiceUser <= db.UsersCount()) {
                                        shared_ptr<User> selectedUser = db.GetIndexUser(choiceUser - 1);
                                        cout << "1. Change Firstname" << endl;
                                        cout << "2. Change Lastname" << endl;
                                        cout << "3. Change Fathername" << endl;
                                        cout << "4. Change Address" << endl;
                                        cout << "5. Change Phone" << endl;
                                        cout << "6. Change Username" << endl;
                                        cout << "7. Change Password" << endl;
                                        int modifyChoice{};
                                        cout << "Enter choice: ";
                                        cin >> modifyChoice;
                                        if (modifyChoice == 1) {
                                            string newFirtsname{};
                                            cout << "Enter new firstname: ";
                                            cin >> newFirtsname;
                                            selectedUser->SetFirstName(newFirtsname);
                                            cout << "Firstname updated successfully!" << endl;
                                            Sleep(1500);
                                            system("cls");
                                        }
                                        else if (modifyChoice == 2) {
                                            string newLastname{};
                                            cout << "Enter new lastname: ";
                                            cin >> newLastname;
                                            selectedUser->SetLastName(newLastname);
                                            cout << "Lastname updated successfully!" << endl;
                                        }
                                        else if (modifyChoice == 3) {
                                            string newFathername{};
                                            cout << "Enter new fathername: ";
                                            cin >> newFathername;
                                            selectedUser->SetFatherName(newFathername);
                                            cout << "Fathername updated successfully!" << endl;
                                            Sleep(1500);
                                            system("cls");
                                        }
                                        else if (modifyChoice == 4) {
                                            string newAddress{};
                                            cout << "Enter new address: ";
                                            cin >> newAddress;
                                            selectedUser->SetAddress(newAddress);
                                            cout << "Address updated successfully!" << endl;
                                            Sleep(1500);
                                            system("cls");
                                        }
                                        else if (modifyChoice == 5) {
                                            string newPhone{};
                                            cout << "Enter new phone: ";
                                            cin >> newPhone;
                                            selectedUser->SetPhone(newPhone);
                                            cout << "Phone updated successfully!" << endl;
                                            Sleep(1500);
                                            system("cls");
                                        }
                                        else if (modifyChoice == 6) {
                                            string newUsername{};
                                            cout << "Enter new username: ";
                                            cin >> newUsername;
                                            selectedUser->SetUsername(newUsername);
                                            cout << "Username updated successfully!" << endl;
                                            Sleep(1500);
                                            system("cls");
                                        }
                                        else if (modifyChoice == 7) {
                                            string newPassword{};
                                            cout << "Enter new password: ";
                                            cin >> newPassword;
                                            selectedUser->SetPassword(newPassword);
                                            cout << "Password updated successfully!" << endl;
                                            Sleep(1500);
                                            system("cls");
                                        }
                                        else {
                                            cout << "Invalid choice!" << endl;
                                            Sleep(1500);
                                            system("cls");
                                        }
                                    }
                                }
                                else if (choiceUser == 5) {
                                    system("cls");
                                    break;
                                }
                                else {
                                    cout << "Invalid user choice!" << endl;
                                }
                            }
                        }
                        else if (choice3 == 3) { // Admin kateqoriyalarla bağlı əməliyyatlar
                            while (true) {
                                cout << "1 - Show all categories" << endl;
                                cout << "2 - Add category" << endl;
                                cout << "3 - Delete category" << endl;
                                cout << "4 - Back" << endl;
                                int choiceCategory{};
                                cout << "Enter choice: ";
                                cin >> choiceCategory;
                                if (choiceCategory == 1) {
                                    db.ShowCategories();
                                    Sleep(1500);
                                    system("cls");
                                }
                                else if (choiceCategory == 2) {
                                    string categoryName{};
                                    cout << "Enter the name of the category you want to add: ";
                                    cin >> categoryName;
                                    shared_ptr<Category> newCategory(new Category(categoryName));

                                    while (true) {
                                        string subCategoryName{};
                                        cout << "Enter the name of the subcategory you want to add (or type 'Stop' to stop): ";
                                        cin >> subCategoryName;
                                        if (subCategoryName == "Stop") {
                                            break;
                                        }
                                        shared_ptr<SubCategory> newSubCategory(new SubCategory(subCategoryName));
                                        newCategory->AddSubCategory(newSubCategory);
                                        cout << "Subcategory " << subCategoryName << " successfully added!" << endl;
                                    }
                                    db.AddCategory(newCategory);
                                    cout << "Category and its subcategories successfully added!" << endl;
                                    db.SaveToFile("data.txt");
                                    Sleep(1500);
                                    system("cls");
                                }
                                else if (choiceCategory == 3) {
                                    db.ShowCategories();
                                    int categoryDelete{};
                                    cout << "Select the number of the category you want to delete: ";
                                    cin >> categoryDelete;
                                    if (categoryDelete > 0 && categoryDelete <= db.CategoriesCount()) {
                                        db.DeleteCategory(categoryDelete - 1);
                                        cout << "Category successfully deleted!" << endl;
                                        db.SaveToFile("data.txt");
                                        Sleep(1500);
                                        system("cls");
                                    }
                                    else {
                                        cout << "Wrong category index!" << endl;
                                        Sleep(1500);
                                        system("cls");
                                    }
                                }
                                else if (choiceCategory == 4) {
                                    system("cls");
                                    break;
                                }
                                else {
                                    cout << "Wrong Choice!" << endl;
                                }
                            }
                        }
                        else if (choice3 == 4) { // Admin testlərlə bağlı əməliyyatlar
                            while (true) {
                                cout << "1 - Show all tests" << endl;
                                cout << "2 - Add question to test" << endl;
                                cout << "3 - Remove question from test" << endl;
                                cout << "4 - Back" << endl;
                                int choiceTest{};
                                cout << "Enter choice: ";
                                cin >> choiceTest;
                                if (choiceTest == 1) {
                                    db.ShowCategories();
                                    int choiceShowTest{};
                                    cout << "Select category to see questions: ";
                                    cin >> choiceShowTest;
                                    cin.ignore();
                                    Sleep(1500);
                                    system("cls");
                                    shared_ptr<Category> selectedCategory = db.GetIndexCategory(choiceShowTest - 1);
                                    if (selectedCategory) {
                                        cout << "------------- " << selectedCategory->GetCategoryName() << " -------------\n\n";
                                        for (size_t i = 0; i < selectedCategory->GetSubCategories().size(); i++)
                                        {
                                            shared_ptr<SubCategory> selectedSubCategory = selectedCategory->GetSubCategories()[i];
                                            selectedSubCategory->ShowQuestions();

                                        }
                                        int chooseBack;
                                        cout << "Press 0 to go back: ";
                                        cin >> chooseBack;
                                        Sleep(1500);
                                        system("cls");
                                    }
                                    else {
                                        cout << "Category not found!" << endl;
                                        Sleep(1500);
                                        system("cls");
                                    }
                                }
                                else if (choiceTest == 2) { // Admin test elave edir
                                    db.ShowCategories();
                                    int choiceTestAdd{};
                                    cout << "Which category would you like to add a question to: ";
                                    cin >> choiceTestAdd;
                                    cin.ignore();
                                    Sleep(1500);
                                    system("cls");
                                    shared_ptr<Category> selectedCategory = db.GetIndexCategory(choiceTestAdd - 1);
                                    if (selectedCategory) {
                                        selectedCategory->ShowSubCategories();
                                        int choiceTestAdd2{};
                                        cout << "Which subcategory would you like to add a question to: ";
                                        cin >> choiceTestAdd2;
                                        cin.ignore();
                                        Sleep(1500);
                                        system("cls");
                                        shared_ptr<SubCategory> selectedSubCategory = selectedCategory->GetSubCategories()[choiceTestAdd2 - 1];
                                        cout << "Select question type:" << endl;
                                        cout << "1 - Open question" << endl;
                                        cout << "2 - Two choice question" << endl;
                                        cout << "3 - Four choice question" << endl;
                                        int choiceQuestionType{};
                                        cout << "Enter choice: ";
                                        cin >> choiceQuestionType;
                                        cin.ignore();
                                        Sleep(1500);
                                        system("cls");
                                        string questionText{}, correctAnswer{};
                                        vector<string> wrongAnswers;

                                        cout << "Enter the question: ";
                                        getline(cin, questionText);

                                        cout << "Enter correct answer: ";
                                        getline(cin, correctAnswer);
                                        if (choiceQuestionType == 1) {
                                            shared_ptr<Question> question = make_shared<OpenQuestion>(questionText, correctAnswer);
                                            selectedSubCategory->AddQuestion(question);
                                            db.SaveToFile("data.txt");
                                        }
                                        else if (choiceQuestionType == 2) {
                                            string wrongAnswer{};
                                            cout << "Enter wrong answer: ";
                                            getline(cin, wrongAnswer);
                                            shared_ptr<Question> question = make_shared<TwoChoiceQuestion>(questionText, correctAnswer, wrongAnswer);
                                            selectedSubCategory->AddQuestion(question);
                                            db.SaveToFile("data.txt");
                                        }
                                        else if (choiceQuestionType == 3) {
                                            string wrongAnswer{};
                                            for (int i = 0; i < 3; ++i) {
                                                cout << "Enter wrong answer " << i + 1 << ": ";
                                                getline(cin, wrongAnswer);
                                                wrongAnswers.push_back(wrongAnswer);
                                            }
                                            shared_ptr<Question> question = make_shared<FourChoiceQuestion>(questionText, correctAnswer, wrongAnswers);
                                            selectedSubCategory->AddQuestion(question);
                                            db.SaveToFile("data.txt");
                                        }

                                        cout << "Question added!" << endl;
                                        Sleep(1500);
                                        system("cls");
                                    }
                                    else {
                                        cout << "Category not found!" << endl;
                                    }
                                }
                                else if (choiceTest == 3) { // Admin test silir
                                    db.ShowCategories();
                                    int choiceTestDelete{};
                                    cout << "Which category would you like to delete a question to: ";
                                    cin >> choiceTestDelete;
                                    cin.ignore();
                                    Sleep(1500);
                                    system("cls");
                                    shared_ptr<Category> selectedCategory = db.GetIndexCategory(choiceTestDelete - 1);
                                    if (selectedCategory) {
                                        selectedCategory->ShowSubCategories();
                                        int choiceTestAdd2{};
                                        cout << "Which subcategory would you like to delete a question to: ";
                                        cin >> choiceTestAdd2;
                                        cin.ignore();
                                        Sleep(1500);
                                        system("cls");
                                        shared_ptr<SubCategory> selectedSubCategory = selectedCategory->GetSubCategories()[choiceTestAdd2 - 1];
                                        if (selectedSubCategory) {
                                            selectedSubCategory->ShowQuestions();
                                            int choiceQuestion;
                                            cout << "Enter the question number you want to delete: ";
                                            cin >> choiceQuestion;
                                            cin.ignore();
                                            Sleep(1500);
                                            system("cls");
                                            if (choiceQuestion >= 1 && choiceQuestion <= selectedSubCategory->GetSubcategoryQuestions().size()) {
                                                selectedSubCategory->DeleteQuestion(choiceQuestion - 1);
                                                db.SaveToFile("data.txt");
                                            }
                                            else {
                                                cout << "You entered the wrong question  number!" << endl;
                                            }
                                        }
                                        else {
                                            cout << "You entered the wrong subcategory number!" << endl;
                                        }
                                    }
                                    else {
                                        cout << "You entered the wrong category number!" << endl;
                                    }
                                }
                                else if (choiceTest == 4) {
                                    Sleep(1000);
                                    system("cls");
                                    break;
                                }
                                else {
                                    cout << "Invalid choice! Please choose between 1-4" << endl;
                                    Sleep(1500);
                                    system("cls");
                                }
                            }

                        }
                        else if (choice3 == 5) {
                            break;
                        }
                        else {
                            cout << "Invalid choice! Please choose between 1-5" << endl;
                            Sleep(1500);
                        }
                    }
                }
                else {
                    cout << "Incorrect Admin username or password!";
                    Sleep(1500);
                }
            }




            else if (choiceLogin == 2) { // USER LOGIN
                system("cls");
                string user_username_login{}, user_password_login{};

                cout << "Enter User username: ";
                cin >> user_username_login;
                cin.ignore();

                cout << "Enter User password: ";
                cin >> user_password_login;
                cin.ignore();

                if (db.UserLogin(user_username_login, user_password_login)) {
                    shared_ptr<User> currentUser;
                    for (auto& user : db.GetUsers()) {
                        if (user->GetUsername() == user_username_login) {
                            currentUser = user;
                            break;
                        }
                    }

                    while (true) {
                        cout << "1 - Show old test results" << endl;
                        cout << "2 - Take a new test" << endl;
                        cout << "3 - Back" << endl;
                        int choiceUser{};
                        cout << "Enter choice: ";
                        cin >> choiceUser;
                        cin.ignore();
                        system("cls");
                        if (choiceUser == 1) {
                            currentUser->ViewTestsResults();
                        }
                        else if (choiceUser == 2) {
                            db.ShowCategories();
                            int choiceWriteTest{};
                            cout << "Select category to take a test: ";
                            cin >> choiceWriteTest;
                            cin.ignore();
                            system("cls");
                            shared_ptr<Category> selectedCategory = db.GetIndexCategory(choiceWriteTest - 1);
                            if (selectedCategory) {
                                currentUser->TakeTest(selectedCategory);
                            }
                        }
                        else if (choiceUser == 3) {

                        }
                    }
                }
                else {
                    cout << "Incorrect username or password!" << endl;
                }
            }
            else {
                cout << "WRONG CHOICE!!!" << endl;
                Sleep(1500);
            }
        }


        else if (choice == 2) { // SIGN UP
            cout << "1.Admin" << endl;
            cout << "2.User" << endl;

            int choice2{};
            cout << "Enter choice: ";
            cin >> choice2;
            cin.ignore();

            if (choice2 == 1) { // ADMIN SIGN UP
                string admin_username{}, admin_password{};

                cout << "Enter Admin username: ";
                cin >> admin_username;
                cin.ignore();

                cout << "Enter Admin password: ";
                cin >> admin_password;
                cin.ignore();

                shared_ptr<Admin> admin(new Admin(admin_username, admin_password));
                db.AddAdmin(admin);
            }
            else if (choice2 == 2) { // USER SIGN UP
                string user_firstname{}, user_lastname{}, user_fathername{}, user_address{}, user_phone{}, user_username{}, user_password{};

                cout << "Enter user firstname: ";
                cin >> user_firstname;
                cin.ignore();
                cout << "Enter user lastname: ";
                cin >> user_lastname;
                cin.ignore();
                cout << "Enter user fathername: ";
                cin >> user_fathername;
                cin.ignore();
                cout << "Enter user address: ";
                cin >> user_address;
                cin.ignore();

                cout << "Enter user phone: ";
                cin >> user_phone;
                cin.ignore();

                cout << "Enter user username: ";
                cin >> user_username;
                cin.ignore();

                cout << "Enter user password: ";
                cin >> user_password;
                cin.ignore();

                shared_ptr<User> user(new User(user_firstname, user_lastname, user_fathername, user_address, user_phone, user_username, user_password));
                db.AddUser(user);
            }
            else {
                cout << "WRONG CHOICE!!!" << endl;
            }
        }
        else if (choice == 3) {
            break;
        }
        else {
            cout << "WRONG CHOICE!!!" << endl;
            Sleep(1500);
        }
    }
}
