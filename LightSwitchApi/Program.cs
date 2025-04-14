var builder = WebApplication.CreateBuilder(args);

var app = builder.Build();

app.UseHttpsRedirection();

app.MapGet("/lightSwitchState", () =>
{
    return LoadStatus();
})
.WithName("GetLightSwitchState");

app.MapPost("/lightSwitchState/toggle", (ToggleRequest request) =>
{
    string correctPassword = app.Configuration["Password"] ?? throw new Exception("Password not set in config");

    if (request.Password != correctPassword)
    {
        return Results.Unauthorized();
    }

    var currStatus = LoadStatus();
    var newStatus = !currStatus;
    SaveStatus(newStatus);

    return Results.Ok(newStatus);
})
.WithName("ToggleLightSwitchState");

app.Run();

static void SaveStatus(bool flag)
{
    File.WriteAllText("flag.txt", flag.ToString());
}

static bool LoadStatus()
{
    var loadedFlag = true;

    if (File.Exists("flag.txt"))
    {
        string content = File.ReadAllText("flag.txt");
        if (bool.TryParse(content, out bool result))
        {
            loadedFlag = result;
        }
    }

    return loadedFlag;
}

record ToggleRequest(string Password);